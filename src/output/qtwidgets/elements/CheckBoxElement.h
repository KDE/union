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

class CheckBoxElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_CheckBoxIndicator> : public TypeHelper<CheckBoxElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_CheckBoxContents> : public TypeHelper<CheckBoxElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_CheckBox> : public TypeHelper<CheckBoxElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_CheckBoxLabel> : public TypeHelper<CheckBoxElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorCheckBox> : public TypeHelper<CheckBoxElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_CheckBox> : public TypeHelper<CheckBoxElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_IndicatorWidth> : public TypeHelper<CheckBoxElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_IndicatorHeight> : public TypeHelper<CheckBoxElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_CheckBoxLabelSpacing> : public TypeHelper<CheckBoxElement, QStyleOptionButton>{};
/* clang-format on */
}
}

class CheckBoxElement : public AbstractElement
{
    Q_OBJECT

public:
    CheckBoxElement(const QStyleOptionButton *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~CheckBoxElement() override;

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
