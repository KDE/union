// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "BackgroundDrawing.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>
#include <qstyleoption.h>

#include "ElementCache.h"

class UnionStyle;
class ScrollBarElement;
namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ComplexControl, QStyle::CC_ScrollBar> : public TypeHelper<ScrollBarElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ScrollBarAddLine> : public TypeHelper<ScrollBarElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ScrollBarSubLine> : public TypeHelper<ScrollBarElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ScrollBarAddPage> : public TypeHelper<ScrollBarElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ScrollBarSubPage> : public TypeHelper<ScrollBarElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ScrollBarSlider> : public TypeHelper<ScrollBarElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ScrollBarFirst> : public TypeHelper<ScrollBarElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ScrollBarLast> : public TypeHelper<ScrollBarElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_ScrollBar> : public TypeHelper<ScrollBarElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ScrollBarExtent> : public TypeHelper<ScrollBarElement, QStyleOptionSlider>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ScrollBarSliderMin> : public TypeHelper<ScrollBarElement, QStyleOptionSlider>{};
/* clang-format on */
}
}

class ScrollBarElement : public AbstractElement
{
    Q_OBJECT

public:
    ScrollBarElement(const QStyleOptionSlider *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~ScrollBarElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subControlRect(QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    QStringList elementHints() const override;
    void drawBackground(QPainter *painter) const override;
    void drawIndicator(QPainter *painter) const override;
    void updateSubElementList() override;
    const QStyleOptionSlider *m_scrollBarOption = nullptr;
    bool m_horizontal;
    qreal extent() const;
    qreal controlThickness() const;
    qreal minimumSize() const;
};
