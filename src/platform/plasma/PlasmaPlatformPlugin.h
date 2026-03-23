// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#pragma once

#include <PlatformPlugin.h>

class PlasmaPlatformPlugin : public Union::PlatformPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.union.PlatformPlugin" FILE "metadata.json")

public:
    PlasmaPlatformPlugin(QObject *parent = nullptr);

    QString defaultInputPlugin() override;
    QString defaultStyleName() override;

    QIcon platformIcon(const QString &name, const QColor &color) override;
};
