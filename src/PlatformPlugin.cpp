// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PlatformPlugin.h"

#include <QIcon>
#include <QStandardPaths>

using namespace Union;
using namespace Qt::StringLiterals;

PlatformPlugin::PlatformPlugin(QObject *parent)
    : Plugin(parent)
{
}

QList<std::filesystem::path> Union::PlatformPlugin::stylePackagePaths()
{
    const auto paths = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, u"union/styles"_s, QStandardPaths::LocateDirectory);
    QList<std::filesystem::path> result;
    std::ranges::transform(paths, std::back_inserter(result), [](const QString &path) {
        return std::filesystem::path(path.toStdString());
    });
    return result;
}

QIcon PlatformPlugin::platformIcon(const QString &name, [[maybe_unused]] const QColor &color)
{
    return QIcon::fromTheme(name);
}

bool PlatformPlugin::smoothScroll()
{
    return true;
}

qreal PlatformPlugin::animationSpeedMultiplier()
{
    return 1.0;
}
