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

private:
    class Private;
    const std::unique_ptr<Private> d;
};

}
