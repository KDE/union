// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PackageHandler.h"

#include <QFile>
#include <QJsonArray>

#include "InputPlugin.h"
#include "PlatformPlugin.h"

#include "union_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

namespace fs = std::filesystem;

class UNION_NO_EXPORT PackageHandler::Private
{
public:
    std::shared_ptr<PlatformPlugin> platform;
};

const char *PackageHandler::PackageErrorCategory::name() const noexcept
{
    return "package_error";
}

std::string Union::PackageHandler::PackageErrorCategory::message(int errorValue) const
{
    switch (static_cast<PackageHandler::Error>(errorValue)) {
    case Error::InvalidPackage:
        return "An invalid package was provided";
    case Error::AlreadyInstalled:
        return "The package is already installed";
    case Error::NotInstalled:
        return "The package is not installed";
    case Error::NotAnUpdate:
        return "The package is not an update";
    case Error::PackageExists:
        return "The package already exists";
    case Error::UnknownInputType:
        return "The package uses an unknown input type";
    default:
        break;
    }

    return "Unknown Error";
}

std::error_condition PackageHandler::PackageErrorCategory::default_error_condition(int errorValue) const noexcept
{
    switch (static_cast<PackageHandler::Error>(errorValue)) {
    case Error::InvalidPackage:
        return std::make_error_condition(std::errc::invalid_argument);
    case Error::AlreadyInstalled:
        return std::make_error_condition(std::errc::file_exists);
    case Error::NotInstalled:
        return std::make_error_condition(std::errc::no_such_file_or_directory);
    case Error::NotAnUpdate:
        return std::make_error_condition(std::errc::operation_not_supported);
    case Error::PackageExists:
        return std::make_error_condition(std::errc::file_exists);
    case Error::UnknownInputType:
        return std::make_error_condition(std::errc::protocol_not_supported);
    default:
        break;
    }

    return std::error_condition(errorValue, *this);
}

const PackageHandler::PackageErrorCategory &PackageHandler::PackageErrorCategory::instance()
{
    static PackageErrorCategory category;
    return category;
}

std::error_code PackageHandler::PackageErrorCategory::make_package_error(PackageHandler::Error error)
{
    return std::error_code(static_cast<int>(error), instance());
}

PackageHandler::PackageHandler(const std::shared_ptr<PlatformPlugin> &platformPlugin)
    : d(std::make_unique<Private>())
{
    d->platform = platformPlugin;
}

PackageHandler::~PackageHandler() = default;

StylePackage PackageHandler::package(const QString &id)
{
    const auto paths = d->platform->stylePackagePaths();
    for (const auto &path : paths) {
        auto packagePath = path / id.toStdString();
        if (!fs::exists(packagePath)) {
            continue;
        }

        auto package = StylePackage{packagePath};
        if (package.isValid()) {
            return package;
        }
    }

    return StylePackage{};
}

QList<StylePackage> PackageHandler::allPackages(OperationFlags flags)
{
    QSet<QString> seenPackages;
    QList<StylePackage> result;

    const auto packagePaths = d->platform->stylePackagePaths();
    for (const auto &packagePath : packagePaths) {
        for (auto entry : fs::directory_iterator(packagePath)) {
            if (!entry.is_directory()) {
                continue;
            }

            auto package = StylePackage{entry.path()};
            if (package.isValid() && !seenPackages.contains(package.id())) {
                if (!flags.testFlag(OperationFlag::IncludeHidden) && package.isHidden()) {
                    continue;
                }

                result.append(package);
                seenPackages.insert(package.id());
            }
        }
    }

    return result;
}

bool PackageHandler::create(StylePackage &destination, const PackageHandler::CreateInfo &info, std::error_code &errorCode)
{
    if (fs::exists(info.path)) {
        errorCode = PackageErrorCategory::make_package_error(Error::PackageExists);
        return false;
    }

    fs::create_directories(info.path, errorCode);
    if (errorCode) {
        return false;
    }

    fs::create_directories(info.path / "contents", errorCode);
    if (errorCode) {
        return false;
    }

    auto inputPlugin = InputPlugin::inputPlugin(info.inputType);
    if (!inputPlugin) {
        errorCode = PackageErrorCategory::make_package_error(Error::UnknownInputType);
        return false;
    }

    QFile metaDataFile(info.path / "metadata.json");
    if (!metaDataFile.open(QIODevice::WriteOnly)) {
        if (metaDataFile.error() == QFile::FileError::PermissionsError) {
            errorCode = std::make_error_code(std::errc::permission_denied);
        } else {
            errorCode = std::make_error_code(std::errc::io_error);
        }
        return false;
    }

    QTextStream stream{&metaDataFile};
    stream << "{\n";
    stream << R"(    "input-type": ")" << info.inputType << "\",\n";
    stream << R"(    "name": ")" << info.name << "\"";
    if (!info.description.isEmpty()) {
        stream << ",\n";
        stream << R"(    "description": ")" << info.description << "\"";
    }

    if (!info.version.isEmpty()) {
        stream << ",\n";
        stream << R"(    "version": ")" << info.version << "\"";
    }
    if (!info.license.isEmpty()) {
        stream << ",\n";
        stream << R"(    "license": ")" << info.license << "\"";
    }
    if (!info.url.isEmpty()) {
        stream << ",\n";
        stream << R"(    "url": ")" << info.url.toString() << "\"";
    }
    if (!info.authors.isEmpty()) {
        stream << ",\n";
        stream << R"(    "authors": [)" << "\n";
        for (int i = 0; i < info.authors.size(); ++i) {
            stream << "        \"" << info.authors.at(i) << "\"";
            if (i < info.authors.size() - 1) {
                stream << ",";
            }
            stream << "\n";
        }
        stream << "    ]";
    }
    stream << "\n";
    stream << "}\n";

    stream.flush();
    metaDataFile.close();

    destination = StylePackage{info.path};
    return inputPlugin->createPackage(destination, errorCode);
}

bool PackageHandler::install(const StylePackage &package, std::error_code &errorCode)
{
    if (!package.isValid()) {
        errorCode = PackageErrorCategory::make_package_error(Error::InvalidPackage);
        return false;
    }

    auto destination = d->platform->stylePackageInstallPath() / package.path().filename();
    if (fs::exists(destination)) {
        errorCode = PackageErrorCategory::make_package_error(Error::AlreadyInstalled);
        return false;
    }

    fs::copy(package.path(), destination, fs::copy_options::recursive, errorCode);
    if (errorCode) {
        return false;
    }

    return true;
}

bool PackageHandler::uninstall(const StylePackage &package, std::error_code &errorCode)
{
    if (!package.isValid()) {
        errorCode = PackageErrorCategory::make_package_error(Error::InvalidPackage);
        return false;
    }

    const auto installPaths = d->platform->stylePackagePaths();
    if (!installPaths.contains(package.path().parent_path())) {
        errorCode = PackageErrorCategory::make_package_error(Error::NotInstalled);
        return false;
    }

    if (!fs::is_directory(package.path())) {
        errorCode = std::make_error_code(std::errc::not_a_directory);
        return false;
    }

    fs::remove_all(package.path(), errorCode);
    if (errorCode) {
        return false;
    }

    return true;
}

bool PackageHandler::update(const StylePackage &updatePackage, std::error_code &errorCode, OperationFlags flags)
{
    if (!updatePackage.isValid()) {
        errorCode = PackageErrorCategory::make_package_error(Error::InvalidPackage);
        return false;
    }

    auto installedPackage = package(updatePackage.id());
    if (!installedPackage.isValid()) {
        errorCode = PackageErrorCategory::make_package_error(Error::NotInstalled);
        return false;
    }

    if (!flags.testFlag(OperationFlag::SkipVersionCheck)) {
        if (installedPackage.version() == updatePackage.version()) {
            errorCode = PackageErrorCategory::make_package_error(Error::NotAnUpdate);
            return false;
        }
    }

    if (!uninstall(installedPackage, errorCode)) {
        return false;
    }

    return install(updatePackage, errorCode);
}
