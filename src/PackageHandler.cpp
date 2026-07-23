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
