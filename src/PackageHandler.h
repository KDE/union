// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>

#include <QUrl>

#include "StylePackage.h"

#include "union_export.h"

namespace Union
{
class PlatformPlugin;

/*!
 * A class that contains operations that can be performed on style packages.
 *
 * This encapsulates things like listing packages, installing and uninstalling
 * and related operations.
 */
class UNION_EXPORT PackageHandler
{
public:
    /*!
     * \enum PackageHandler::Error
     *
     * Errors that can occur during a package operation.
     *
     * \value None
     *      No error occurred and the operation was successful.
     * \value InvalidPackage
     *      The operation failed because an invalid package was provided.
     * \value AlreadyInstalled
     *      The operation failed because the provided package was already
     *      installed.
     * \value FilesystemError
     *      The operation failed because a file system error occurred during the
     *      operation. Note that this can be anything from insufficient
     *      permissions to disk failure.
     * \value NotInstalled
     *      The operation failed because the provided package is not installed.
     * \value NotAnUpdate
     *      The operation failed because the provided package is not considered
     *      to be an update for the existing installed package.
     * \value PackageExists
     *      The operation failed because a package already exists at the
     *      location provided. Note that this differs from AlreadyInstalled in
     *      that this may also occur for non-install paths.
     * \value UnknownInputType
     *      The operation failed because an unknown input type was requested.
     */
    enum class Error {
        None,
        InvalidPackage,
        AlreadyInstalled,
        FilesystemError,
        NotInstalled,
        NotAnUpdate,
        PackageExists,
        UnknownInputType,
    };

    struct CreateInfo {
        std::filesystem::path path;
        QString inputType;
        QString name;
        QString description;
        QString version;
        QString license;
        QUrl url;
        QStringList authors;
    };

    PackageHandler(const std::shared_ptr<PlatformPlugin> &platformPlugin);
    ~PackageHandler();

    /*!
     * Returns the package with id \p id or an invalid package if it cannot be found.
     */
    StylePackage package(const QString &id);

    /*!
     * Returns a list of all installed packages.
     */
    QList<StylePackage> allPackages();

    /*!
     * Create a new package.
     */
    Error create(StylePackage &destination, const CreateInfo &info);
    /*!
     * Install a package.
     *
     * This will install the package \p package if it is valid by copying the
     * contents to a platform-specific installation directory. It will return
     * Error::None if installation was successful. Installation
     * will fail and return Error::AlreadyInstalled if the package is already
     * installed. It may also return Error::FilesystemError if any operating
     * system error occurs.
     */
    Error install(const StylePackage &package);
    /*!
     * Uninstall a package.
     *
     * This will uninstall the package \p package if it is currently installed.
     * It will return Error::None if uninstallation was successful. It will
     * return Error::NotInstalled if the package is not installed. It will
     * return Error::FilesystemError if an operating system error occurs.
     */
    Error uninstall(const StylePackage &package);
    /*!
     * Update a package.
     *
     * This will update an installed package with the same ID as \p updatePackage
     * and replace its contents with the contents of \p updatePackage. It will
     * return Error::None if the update was successful. It will return
     * Error::NotInstalled if no installed package with the same ID could be
     * found. It will return Error::NotAnUpdate if the given package is not
     * considered to be an update for the installed package.
     *
     * Updating happens by first uninstalling the existing package and then
     * installing \p updatePackage. Any error that occurs during these steps
     * will cause the update to fail and will return the error.
     */
    Error update(const StylePackage &updatePackage);

private:
    class Private;
    const std::unique_ptr<Private> d;
};

}
