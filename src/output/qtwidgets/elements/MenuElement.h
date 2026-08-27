// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class MenuElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelMenu> : public TypeHelper<MenuElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameMenu> : public TypeHelper<MenuElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_Menu> : public TypeHelper<MenuElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_MenuHMargin> : public TypeHelper<MenuElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_MenuVMargin> : public TypeHelper<MenuElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_MenuPanelWidth> : public TypeHelper<MenuElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_MenuDesktopFrameWidth> : public TypeHelper<MenuElement, QStyleOption>{};
/* clang-format on */
}
}

class MenuElement : public AbstractElement
{
    Q_OBJECT

public:
    MenuElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~MenuElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    void updateSubElementList() override;
    const QStyleOption *m_menuOption = nullptr;
};
