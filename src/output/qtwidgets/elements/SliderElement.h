// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class SliderElement : public AbstractElement
{
    Q_OBJECT

public:
    SliderElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;

    QRectF subControlRect(const QStyleOption *option, QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const override;
    qreal pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

private:
    Union::IdentifierList elementHints() const override;
    void drawBackground(QPainter *painter) const override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    const QStyleOptionSlider *m_sliderOption = nullptr;
    bool m_isHorizontal;
    bool m_isInverted;
    bool m_isReverse;
    QList<QRectF> tickLines() const;
    qreal controlThickness() const;
    QSizeF tickMarkSize() const;

    Union::ElementList m_tickElementList;
    Union::StylePropertyGroupPtr m_tickProperties = nullptr;
    Union::StylePropertyGroupPtr m_highlightedTickProperties = nullptr;
    Union::ElementList m_indicatorElementList;
    Union::StylePropertyGroupPtr m_indicatorProperties = nullptr;
    Union::ElementList m_contentElementList;
    Union::StylePropertyGroupPtr m_contentProperties = nullptr;
};
}
}
