// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "BackgroundDrawing.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class SliderElement;
namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ComplexControl, QStyle::CC_Slider> : public TypeHelper<SliderElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_Slider> : public TypeHelper<SliderElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_SliderLength> : public TypeHelper<SliderElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_SliderThickness> : public TypeHelper<SliderElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_SliderControlThickness> : public TypeHelper<SliderElement, QStyleOptionSlider>{};
/* clang-format on */
}
}

class SliderElement : public AbstractElement
{
    Q_OBJECT

public:
    SliderElement(const QStyleOptionSlider *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~SliderElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subControlRect(QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    QStringList elementHints() const override;
    void updateSubElementList() override;
    void drawBackground(QPainter *painter) const override;
    const QStyleOptionSlider *m_sliderOption = nullptr;
    bool m_isHorizontal;
    bool m_isInverted;
    bool m_isReverse;
    QList<QRect> tickLines() const;
    qreal controlThickness() const;
};
