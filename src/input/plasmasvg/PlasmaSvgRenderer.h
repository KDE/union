/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>

#include <QPalette>
#include <QSvgRenderer>

#include <KColorScheme>

#include <Element.h>
#include <Theme.h>

class PlasmaSvgRenderer
{
public:
    struct RendererId {
        Union::Theme *theme;
        QString path;
        QPalette::ColorGroup colorGroup;
        Union::Element::ColorSet colorSet;

        inline bool operator==(const RendererId &other) const
        {
            return path == other.path && colorGroup == other.colorGroup && colorSet == other.colorSet;
        }
    };

    PlasmaSvgRenderer();

    QPalette::ColorGroup colorGroup() const;
    void setColorGroup(QPalette::ColorGroup group);

    KColorScheme::ColorSet colorSet() const;
    void setColorSet(KColorScheme::ColorSet colorSet);

    QString path() const;
    void setPath(const QString &path);

    bool load();

    bool elementExists(const QString &elementName);
    QRectF elementRect(const QString &elementName);
    void render(QPainter *painter, const QString &elementName, const QRectF &bounds = QRectF{});

    static std::shared_ptr<PlasmaSvgRenderer> rendererForPath(const std::shared_ptr<Union::Theme> &theme, //
                                                              QAnyStringView path,
                                                              QPalette::ColorGroup colorGroup,
                                                              Union::Element::ColorSet colorSet);
    static void clearRenderers(const std::shared_ptr<Union::Theme> &theme);

private:
    QString createStylesheet();

    QPalette::ColorGroup m_colorGroup = QPalette::Normal;
    KColorScheme::ColorSet m_colorSet = KColorScheme::ColorSet::Window;
    QString m_path;
    std::unique_ptr<QSvgRenderer> m_renderer;

    inline static QHash<RendererId, std::shared_ptr<PlasmaSvgRenderer>> s_rendererCache;
};
