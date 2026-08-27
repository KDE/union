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
class SpinBoxElement;
namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ComplexControl, QStyle::CC_SpinBox> : public TypeHelper<SpinBoxElement, QStyleOptionSpinBox>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorSpinPlus> : public TypeHelper<SpinBoxElement, QStyleOptionSpinBox>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorSpinMinus> : public TypeHelper<SpinBoxElement, QStyleOptionSpinBox>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorSpinUp> : public TypeHelper<SpinBoxElement, QStyleOptionSpinBox>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorSpinDown> : public TypeHelper<SpinBoxElement, QStyleOptionSpinBox>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_SpinBox> : public TypeHelper<SpinBoxElement, QStyleOptionSpinBox>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_SpinBoxFrameWidth> : public TypeHelper<SpinBoxElement, QStyleOptionSpinBox>{};
/* clang-format on */
}
}

class SpinBoxElement : public AbstractElement
{
    Q_OBJECT

public:
    SpinBoxElement(const QStyleOptionSpinBox *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~SpinBoxElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

    QRectF subControlRect(QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;

private:
    QStringList elementHints() const override;
    void updateSubElementList() override;
    void drawSpinIndicator(QPainter *painter, const QStyle::PrimitiveElement &primitive) const;
    const QStyleOptionSpinBox *m_spinBoxOption = nullptr;
    bool m_hasButtons;
};
