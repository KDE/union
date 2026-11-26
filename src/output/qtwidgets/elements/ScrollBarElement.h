// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class ScrollBarElement : public AbstractElement
{
    Q_OBJECT

public:
    ScrollBarElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;

    std::optional<QRectF> subControlRect(const QStyleOption *option, QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const override;
    std::optional<qreal> pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

private:
    Union::IdentifierList elementHints() const override;
    void drawBackground(QPainter *painter) const override;
    void drawIndicator(QPainter *painter) const override;
    void updateSubElementList() override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    const QStyleOptionSlider *m_scrollBarOption = nullptr;
    bool m_horizontal;
    qreal extent() const;
    qreal controlThickness() const;
    qreal minimumSize() const;

    Union::ElementList m_indicatorElementList;
    Union::StylePropertyGroupPtr m_indicatorProperties = nullptr;
};
}
}
