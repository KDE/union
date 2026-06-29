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
    /*!
     * Returns whether smooth scrolling is enabled on this platform.
     *
     * Enables/disables tweened scrollbars and inertial scrolling. Defaults to true
     */
    virtual bool smoothScroll();
    /*!
     * Emitted whenever smoothScroll changes.
     */
    Q_SIGNAL void smoothScrollChanged();
    /*!
     * Returns a multiplier to globally change the animation speed.
     *
     * This value should be used with animation speeds to ensure a platform's
     * preferences with regards to animation speed is respected.
     *
     * By default this will return 1.0.
     */
    virtual qreal animationSpeedMultiplier();
    /*!
     * Emitted whenever animationSpeedMultiplier changes.
     */
    Q_SIGNAL void animationSpeedMultiplierChanged();
};

}

Q_DECLARE_INTERFACE(Union::PlatformPlugin, "org.kde.union.PlatformPlugin")
Q_DECLARE_METATYPE(Union::PlatformPlugin)
