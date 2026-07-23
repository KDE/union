// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#pragma once

#include <KConfigWatcher>

#include <PlatformPlugin.h>

class PlasmaPlatformPlugin : public Union::PlatformPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.union.PlatformPlugin" FILE "metadata.json")

public:
    PlasmaPlatformPlugin(QObject *parent = nullptr);

    QString defaultStyleName() override;

    QIcon platformIcon(const QString &name, const QColor &color) override;

    bool smoothScroll() override;

    qreal animationSpeedMultiplier() override;

private:
    void setSmoothScroll(bool enabled);
    void setAnimationSpeedMultiplier(qreal multiplier);

    bool m_smoothScroll = true;
    qreal m_animationSpeedMultiplier = 1.0;
    KConfigWatcher::Ptr m_kdeGlobalsWatcher;
};
