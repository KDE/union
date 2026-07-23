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

QList<StylePackage> PackageHandler::allPackages()
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
                result.append(package);
                seenPackages.insert(package.id());
            }
        }
    }

    return result;
}

PackageHandler::Error PackageHandler::create(StylePackage &destination, const PackageHandler::CreateInfo &info)
{
    if (fs::exists(info.path)) {
        return Error::PackageExists;
    }

    if (!fs::create_directories(info.path)) {
        return Error::FilesystemError;
    }

    if (!fs::create_directories(info.path / "contents")) {
        return Error::FilesystemError;
    }

    auto inputPlugin = InputPlugin::inputPlugin(info.inputType);
    if (!inputPlugin) {
        return Error::UnknownInputType;
    }

    QFile metaDataFile(info.path / "metadata.json");
    if (!metaDataFile.open(QIODevice::WriteOnly)) {
        return Error::FilesystemError;
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
    return inputPlugin->createPackage(destination);
}

PackageHandler::Error PackageHandler::install(const StylePackage &package)
{
    if (!package.isValid()) {
        return Error::InvalidPackage;
    }

    auto destination = d->platform->stylePackageInstallPath() / package.path().filename();
    if (fs::exists(destination)) {
        return Error::AlreadyInstalled;
    }

    std::error_code ec;
    fs::copy(package.path(), destination, fs::copy_options::recursive, ec);
    if (ec) {
        return Error::FilesystemError;
    }

    return Error::None;
}

PackageHandler::Error PackageHandler::uninstall(const StylePackage &package)
{
    if (!package.isValid()) {
        return Error::InvalidPackage;
    }

    const auto installPaths = d->platform->stylePackagePaths();
    if (!installPaths.contains(package.path().parent_path())) {
        return Error::NotInstalled;
    }

    std::error_code ec;
    fs::remove_all(package.path(), ec);
    if (ec) {
        return Error::FilesystemError;
    }

    return Error::None;
}

PackageHandler::Error PackageHandler::update(const StylePackage &updatePackage)
{
    if (!updatePackage.isValid()) {
        return Error::InvalidPackage;
    }

    auto installedPackage = package(updatePackage.id());
    if (!installedPackage.isValid()) {
        return Error::NotInstalled;
    }

    if (installedPackage.version() == updatePackage.version()) {
        return Error::NotAnUpdate;
    }

    if (auto result = uninstall(installedPackage); result != Error::None) {
        return result;
    }

    return install(updatePackage);
}
