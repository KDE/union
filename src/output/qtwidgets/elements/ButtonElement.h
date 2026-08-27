// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;

class ButtonElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_PushButtonFocusRect> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_PushButtonContents> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_PushButtonBevel> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_PushButton> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_PushButtonBevel> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_PushButtonLabel> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameDefaultButton> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameButtonBevel> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelButtonCommand> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelButtonBevel> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_PushButton> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ButtonMargin> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ButtonDefaultIndicator> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ButtonIconSize> : public TypeHelper<ButtonElement, QStyleOptionButton>{};
/* clang-format on */
}
}

class ButtonElement : public AbstractElement
{
    Q_OBJECT

public:
    ButtonElement(const QStyleOptionButton *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~ButtonElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subElementRect(QStyle::SubElement element) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    void updateSubElementList() override;
    QStringList elementHints() const override;
    Union::Element::States elementStates() const override;

    const QStyleOptionButton *m_buttonOption = nullptr;
};
