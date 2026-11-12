// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include <ElementQuery.h>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsEffect>
#include <QIcon>
#include <QObject>
#include <QStyleOption>

namespace Union
{
namespace Widgets
{

class UnionGraphicsEffect : public QGraphicsDropShadowEffect
{
    Q_OBJECT
public:
    UnionGraphicsEffect(QObject *parent);
    ~UnionGraphicsEffect() override;

    void setProperties(Union::StylePropertyGroupPtr properties);
    void draw(QPainter *painter) override;
    void setDrawRect(const QRectF &rect);

    QRectF boundingRectFor(const QRectF &rect) const override;

private:
    Union::StylePropertyGroupPtr m_properties;
    bool m_isValid;
    QColor m_outlineColor;
    QMarginsF m_outlineSize;
    QRectF m_drawRect;
};
}
}
