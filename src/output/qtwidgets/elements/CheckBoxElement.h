// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "ElementCache.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

namespace Union
{
namespace Widgets
{

class UnionStyle;
class CheckBoxElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_CheckBoxIndicator> : public TypeHelper<CheckBoxElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_CheckBoxContents> : public TypeHelper<CheckBoxElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_CheckBox> : public TypeHelper<CheckBoxElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_CheckBoxLabel> : public TypeHelper<CheckBoxElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorCheckBox> : public TypeHelper<CheckBoxElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_CheckBox> : public TypeHelper<CheckBoxElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_IndicatorWidth> : public TypeHelper<CheckBoxElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_IndicatorHeight> : public TypeHelper<CheckBoxElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_CheckBoxLabelSpacing> : public TypeHelper<CheckBoxElement, QStyleOption>{};
/* clang-format on */
}
}

class CheckBoxElement : public AbstractElement
{
    Q_OBJECT

public:
    CheckBoxElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~CheckBoxElement() override;

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    void layout() override;
    QRectF subElementRect(QStyle::SubElement element) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    void updateSubElementList() override;
    void drawIndicator(QPainter *painter) const override;
    Union::IdentifierList elementHints() const override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    const QStyleOptionButton *m_buttonOption = nullptr;
    Union::ElementList m_indicatorElementList;
    Union::StylePropertyGroupPtr m_indicatorProperties = nullptr;
};

}
}
