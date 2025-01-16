// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <memory>

#include <QObject>

#include "union_export.h"

namespace Union
{

class Theme;
class ThemeRegistryPrivate;

/**
 * A class to load and keep track of Theme instances.
 *
 * This can be considered to be the main entry point to Union. It loads input
 * plugins and uses those to create new Theme instances. It also handles caching
 * of the Theme instances.
 */
class UNION_EXPORT ThemeRegistry : public QObject
{
    Q_OBJECT

public:
    ThemeRegistry(std::unique_ptr<ThemeRegistryPrivate> &&d);
    ~ThemeRegistry() override;

    /**
     * Load any cached data from disk.
     */
    void load();
    /**
     * Save data to disk for caching.
     */
    void save();
    /**
     * Get the default Theme instance as determined by the current platform.
     *
     * \return A Theme instance for the current default theme.
     */
    std::shared_ptr<Theme> defaultTheme();
    /**
     * Get a theme instance by name.
     *
     * \param themeName The name of the theme to retrieve.
     * \param pluginName The name of the input plugin that provides the theme. If
     *                   empty, the theme will be searched for in available plugins.
     *
     * \return A Theme instance that matches `themeName` and `pluginName`, or
     * `nullptr` if it could not be found.
     */
    std::shared_ptr<Theme> theme(const QString &themeName, const QString &pluginName = QString{});

    static std::shared_ptr<ThemeRegistry> instance();

private:
    static void cleanup();

    const std::unique_ptr<ThemeRegistryPrivate> d;
};

}
