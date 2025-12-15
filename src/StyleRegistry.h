// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <memory>

#include <QObject>

#include "union_export.h"

namespace Union
{

class Style;
class StyleRegistryPrivate;

/*!
 * \class Union::StyleRegistry
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief A class to load and keep track of Style instances.
 *
 * This can be considered to be the main entry point to Union. It loads input
 * plugins and uses those to create new Style instances. It also handles caching
 * of the Style instances.
 */
class UNION_EXPORT StyleRegistry : public QObject
{
    Q_OBJECT

public:
    StyleRegistry(std::unique_ptr<StyleRegistryPrivate> &&d);
    ~StyleRegistry() override;

    /*!
     * Load any cached data from disk.
     */
    void load();
    /*!
     * Save data to disk for caching.
     */
    void save();
    /*!
     * Get the default Style instance as determined by the current platform.
     *
     * Returns a Style instance for the current default theme.
     */
    std::shared_ptr<Style> defaultStyle();
    /*!
     * Get a style instance by name.
     *
     * \a styleName  The name of the style to retrieve.
     * \a pluginName The name of the input plugin that provides the style. If
     *               empty, the style will be searched for in available plugins.
     *
     * Returns a Style instance that matches `styleName` and `pluginName`, or
     * `nullptr` if it could not be found.
     */
    std::shared_ptr<Style> style(const QString &styleName, const QString &pluginName = QString{});

    /*!
     * Returns the global instance of StyleRegistry.
     */
    static std::shared_ptr<StyleRegistry> instance();

private:
    static void cleanup();

    const std::unique_ptr<StyleRegistryPrivate> d;
};

}
