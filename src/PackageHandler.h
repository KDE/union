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
    Q_GADGET
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
        InvalidPackage = 1,
        AlreadyInstalled,
        NotInstalled,
        NotAnUpdate,
        PackageExists,
        UnknownInputType,
    };

    class PackageErrorCategory : public std::error_category
    {
    public:
        const char *name() const noexcept override;
        std::string message(int errorValue) const override;
        std::error_condition default_error_condition(int errorValue) const noexcept override;

        static const PackageErrorCategory &instance();
        static std::error_code make_package_error(Error error);
    };

    /*!
     * \enum PackageHandler::OperationFlag
     *
     * Flags for various package operations.
     *
     * \value IncludeHidden
     *      Include hidden packages in listings.
     * \value SkipVersionCheck
     *      Skip the version check for updates and install even if the package
     *      is the same or an older version.
     */
    enum class OperationFlag {
        IncludeHidden = 1 << 0,
        SkipVersionCheck = 1 << 1,
    };
    Q_DECLARE_FLAGS(OperationFlags, OperationFlag)
    Q_FLAG(OperationFlags)

    /*!
     * A struct containing data for the create() method.
     */
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
    QList<StylePackage> allPackages(OperationFlags flags = {});

    /*!
     * Create a new package.
     */
    bool create(StylePackage &destination, const CreateInfo &info, std::error_code &errorCode);
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
    bool install(const StylePackage &package, std::error_code &errorCode);
    /*!
     * Uninstall a package.
     *
     * This will uninstall the package \p package if it is currently installed.
     * It will return Error::None if uninstallation was successful. It will
     * return Error::NotInstalled if the package is not installed. It will
     * return Error::FilesystemError if an operating system error occurs.
     */
    bool uninstall(const StylePackage &package, std::error_code &errorCode);
    /*!
     * Update a package.
     *
     * This will update an installed package with the same ID as \p updatePackage
     * and replace its contents with the contents of \p updatePackage. It will
     * return Error::None if the update was successful. It will return
     * Error::NotInstalled if no installed package with the same ID could be
     * found. It will return Error::NotAnUpdate if the given package is not
     * considered to be an update for the installed package and \a ignore is not
     * set to IgnoreVersion::Ignore.
     *
     * Updating happens by first uninstalling the existing package and then
     * installing \p updatePackage. Any error that occurs during these steps
     * will cause the update to fail and will return the error.
     */
    bool update(const StylePackage &updatePackage, std::error_code &errorCode, OperationFlags flags = {});

private:
    class Private;
    const std::unique_ptr<Private> d;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Union::PackageHandler::OperationFlags);
