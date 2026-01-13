// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#pragma once

// #include <QCache>

#include <KColorScheme>
#include <KConfigGroup>

#include "Color.h"
#include "LruCache.h"

class KColorSchemeProvider : public Union::ColorProvider
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.union.ColorProvider" FILE "kcolorscheme-metadata.json")

public:
    KColorSchemeProvider(QObject *parent = nullptr);

    std::optional<Union::ColorProvider::Rgba> color(const QStringList &arguments) const override;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    mutable Union::LruCache<qsizetype, Union::ColorProvider::Rgba> m_cache;
    KSharedConfigPtr m_colorConfig;
};
