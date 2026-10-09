// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#pragma once

#include "ShadowTileMap.h"
#include <KConfigWatcher>
#include <KWindowShadow>
#include <QWindow>

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

    // QWidget Window shadows
    void installWindowShadow(QWidget *widget, const QImage &shadowImage, const QMargins &shadowMargins) override;
    void uninstallWindowShadow(QObject *object) override;
    void drawMdiShadow(QPainter *painter, const QRect paintRect) override;

private:
    Q_SLOT void setSmoothScroll(bool enabled);
    void setAnimationSpeedMultiplier(qreal multiplier);
    void setDefaultStyle(const QString &style);

    bool m_smoothScroll = true;
    qreal m_animationSpeedMultiplier = 1.0;
    QString m_defaultStyle;
    KConfigWatcher::Ptr m_kdeGlobalsWatcher;
    KConfigWatcher::Ptr m_unionSettingsWatcher;

    // Create shadow tiles for QWidget Window shadows
    const QVector<KWindowShadowTile::Ptr> &createShadowTiles(const QVector<QPixmap> &pixmaps);
    // Managed shadows
    QMap<QWindow *, KWindowShadow *> m_shadows;
    // Shared shadow tiles
    Union::ShadowTileMap m_shadowTiles;
    QVector<KWindowShadowTile::Ptr> m_tiles;
    // There are 8 tiles in total
    const int tileAmount = 8;
};
