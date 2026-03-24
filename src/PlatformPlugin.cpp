// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PlatformPlugin.h"

#include <QIcon>

using namespace Union;
using namespace Qt::StringLiterals;

PlatformPlugin::PlatformPlugin(QObject *parent)
    : Plugin(parent)
{
}

QString PlatformPlugin::defaultInputPlugin()
{
    return QString{};
}

QIcon PlatformPlugin::platformIcon(const QString &name, [[maybe_unused]] const QColor &color)
{
    return QIcon::fromTheme(name);
}
