// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#include "PlasmaPlatformPlugin.h"

#include <QGuiApplication>
#include <QIcon>

#include <KIconColors>
#include <KRuntimePlatform>

using namespace Qt::StringLiterals;

PlasmaPlatformPlugin::PlasmaPlatformPlugin(QObject *parent)
    : Union::PlatformPlugin(parent)
{
}

QString PlasmaPlatformPlugin::defaultInputPlugin()
{
    return u"css"_s;
}

QString PlasmaPlatformPlugin::defaultStyleName()
{
    // These values should really be queries by the CSS using media queries,
    // however those are currently unimplemented in cxx-rust-cssparser and will
    // take some time to implement. So until that time, we switch the default
    // style based on these values.
    if (KRuntimePlatform::runtimePlatform().contains(u"phone"_s)) {
        return u"breeze-mobile"_s;
    }

    if (qEnvironmentVariableIsSet("QT_QUICK_CONTROLS_MOBILE")) {
        return u"breeze-mobile"_s;
    }

    if (qGuiApp->layoutDirection() == Qt::LayoutDirection::RightToLeft) {
        return u"breeze-rtl"_s;
    }

    // TODO: Read from config
    return u"breeze"_s;
}

QIcon PlasmaPlatformPlugin::platformIcon(const QString &name, const QColor &color)
{
    if (color != Qt::transparent) {
        KIconColors colors;
        colors.setText(color);
        return KDE::icon(name, colors);
    } else {
        return KDE::icon(name);
    }
}
