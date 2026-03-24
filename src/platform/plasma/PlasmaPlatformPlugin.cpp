// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#include "PlasmaPlatformPlugin.h"

#include <QIcon>

#include <KIconColors>

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
