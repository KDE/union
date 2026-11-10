// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "ElementCache.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

class UnionStyle;

class ButtonElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_PushButtonFocusRect> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_PushButtonContents> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_PushButtonBevel> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_PushButton> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_PushButtonBevel> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_PushButtonLabel> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameDefaultButton> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameButtonBevel> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelButtonCommand> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelButtonBevel> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_PushButton> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ButtonMargin> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ButtonDefaultIndicator> : public TypeHelper<ButtonElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ButtonIconSize> : public TypeHelper<ButtonElement, QStyleOption>{};
/* clang-format on */
}
}

class ButtonElement : public AbstractElement
{
    Q_OBJECT

public:
    ButtonElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~ButtonElement() override;

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subElementRect(QStyle::SubElement element) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    void updateSubElementList() override;
    Union::IdentifierList elementHints() const override;
    Union::Element::States elementStates() const override;
    QString fallbackElementName(const QStyleOption *opt) const override;

    const QStyleOptionButton *m_buttonOption = nullptr;
    Union::ElementList m_indicatorElementList;
    Union::StylePropertyGroupPtr m_indicatorProperties = nullptr;
};
