/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "UnionPlugin.h"

#include "Background.h"
#include "Element.h"

UnionPlugin::UnionPlugin(QObject *parent)
    : QQmlExtensionPlugin(parent)
{
}

void UnionPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QString::fromLatin1(uri) == QLatin1String("org.kde.union"));

    qmlRegisterType<Union::Background>(uri, 1, 0, "Background");
    qmlRegisterType<Union::Element>(uri, 1, 0, "Element");
}
