// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "UnionStylePlugin.h"

#include "UnionStyle.h"

#include <QApplication>

StylePlugin::StylePlugin(QObject *parent)
    : QStylePlugin(parent)
{
}

QStyle *StylePlugin::create(const QString &key)
{
    if (key.compare(QLatin1String("union"), Qt::CaseInsensitive) == 0) {
        return new UnionStyle;
    }
    return nullptr;
}

QStringList StylePlugin::keys() const
{
    return QStringList(QStringLiteral("Union"));
}

#include "moc_UnionStylePlugin.cpp"
