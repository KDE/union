// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>

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
     */
    enum class Error {
        None,
        InvalidPackage,
        AlreadyInstalled,
        FilesystemError,
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

private:
    class Private;
    const std::unique_ptr<Private> d;
};

}
