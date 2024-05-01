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

class PlasmaSvgRenderer
{
public:
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

private:
    QString createStylesheet();

    QPalette::ColorGroup m_colorGroup = QPalette::Normal;
    KColorScheme::ColorSet m_colorSet = KColorScheme::ColorSet::Window;
    QString m_path;
    std::unique_ptr<QSvgRenderer> m_renderer;
};
