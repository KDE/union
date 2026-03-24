// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include "PluginRegistry.h"

#include "union_export.h"

namespace Union
{
class ColorProvider;

/*!
 * \class Union::PlatformPlugin
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief Abstract base class for plugins providing platform integration.
 *
 */
class UNION_EXPORT PlatformPlugin : public Plugin
{
    Q_OBJECT

public:
    PlatformPlugin(QObject *parent = nullptr);

    /*!
     * Returns the platform's default input plugin.
     *
     * By default, this will return an empty string, which indicates all plugins
     * should be searched for the default style name.
     */
    virtual QString defaultInputPlugin();
    /*!
     * Returns the platform's default style name.
     */
    virtual QString defaultStyleName() = 0;
    /*!
     * Returns an icon from the platform.
     *
     * This should return an icon matching \a name using \a color to recolor
     * the icon, if recoloring is supported. The default implementation will
     * return the result of \c{QIcon::fromTheme()} and does not support
     * recoloring.
     */
    virtual QIcon platformIcon(const QString &name, const QColor &color);
};

}

Q_DECLARE_INTERFACE(Union::PlatformPlugin, "org.kde.union.PlatformPlugin")
Q_DECLARE_METATYPE(Union::PlatformPlugin)
