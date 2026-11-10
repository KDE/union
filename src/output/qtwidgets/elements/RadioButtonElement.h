// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
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
		template<> struct EnumToType<QStyle::SubElement, QStyle::SE_RadioButtonContents> : public TypeHelper<RadioButtonElement, QStyleOption>{};
		template<> struct EnumToType<QStyle::SubElement, QStyle::SE_RadioButtonIndicator> : public TypeHelper<RadioButtonElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_RadioButton> : public TypeHelper<RadioButtonElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_RadioButtonLabel> : public TypeHelper<RadioButtonElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorRadioButton> : public TypeHelper<RadioButtonElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_RadioButton> : public TypeHelper<RadioButtonElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ExclusiveIndicatorWidth> : public TypeHelper<RadioButtonElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ExclusiveIndicatorHeight> : public TypeHelper<RadioButtonElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_RadioButtonLabelSpacing> : public TypeHelper<RadioButtonElement, QStyleOption>{};
/* clang-format on */
}
}

class RadioButtonElement : public AbstractElement
{
    Q_OBJECT

public:
    RadioButtonElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~RadioButtonElement() override;

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
