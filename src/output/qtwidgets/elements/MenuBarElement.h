// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class MenuBarElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_MenuBarEmptyArea> : public TypeHelper<MenuBarElement, QStyleOptionMenuItem>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelMenuBar> : public TypeHelper<MenuBarElement, QStyleOptionMenuItem>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_MenuBar> : public TypeHelper<MenuBarElement, QStyleOptionMenuItem>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_MenuBarVMargin> : public TypeHelper<MenuBarElement, QStyleOptionMenuItem>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_MenuBarHMargin> : public TypeHelper<MenuBarElement, QStyleOptionMenuItem>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_MenuBarPanelWidth> : public TypeHelper<MenuBarElement, QStyleOptionMenuItem>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_MenuBarItemSpacing> : public TypeHelper<MenuBarElement, QStyleOptionMenuItem>{};
/* clang-format on */
}
}

class MenuBarElement : public AbstractElement
{
    Q_OBJECT

public:
    MenuBarElement(const QStyleOptionMenuItem *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~MenuBarElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    const QStyleOptionMenuItem *m_menuItemOption = nullptr;
};
