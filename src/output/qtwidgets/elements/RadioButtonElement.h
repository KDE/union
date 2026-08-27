// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "BackgroundDrawing.h"
#include "ElementCache.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

class UnionStyle;

class RadioButtonElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
		template<> struct EnumToType<QStyle::SubElement, QStyle::SE_RadioButtonContents> : public TypeHelper<RadioButtonElement, QStyleOptionButton>{};
		template<> struct EnumToType<QStyle::SubElement, QStyle::SE_RadioButtonIndicator> : public TypeHelper<RadioButtonElement, QStyleOptionButton>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_RadioButton> : public TypeHelper<RadioButtonElement, QStyleOptionButton>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_RadioButtonLabel> : public TypeHelper<RadioButtonElement, QStyleOptionButton>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorRadioButton> : public TypeHelper<RadioButtonElement, QStyleOptionButton>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_RadioButton> : public TypeHelper<RadioButtonElement, QStyleOptionButton>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ExclusiveIndicatorWidth> : public TypeHelper<RadioButtonElement, QStyleOptionButton>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ExclusiveIndicatorHeight> : public TypeHelper<RadioButtonElement, QStyleOptionButton>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_RadioButtonLabelSpacing> : public TypeHelper<RadioButtonElement, QStyleOptionButton>{};
/* clang-format on */
}
}

class RadioButtonElement : public AbstractElement
{
    Q_OBJECT

public:
    RadioButtonElement(const QStyleOptionButton *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~RadioButtonElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    QRectF subElementRect(QStyle::SubElement element) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    void updateSubElementList() override;
    void drawIndicator(QPainter *painter) const override;
    QStringList elementHints() const override;
    const QStyleOptionButton *m_buttonOption = nullptr;
};
