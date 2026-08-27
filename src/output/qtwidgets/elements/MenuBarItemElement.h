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
class MenuBarItemElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_MenuBarItem> : public TypeHelper<MenuBarItemElement, QStyleOptionMenuItem>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_MenuBarItem> : public TypeHelper<MenuBarItemElement, QStyleOptionMenuItem>{};
/* clang-format on */
}
}

class MenuBarItemElement : public AbstractElement
{
    Q_OBJECT

public:
    MenuBarItemElement(const QStyleOptionMenuItem *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~MenuBarItemElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    void layout() override;

private:
    QStringList elementHints() const override;
    void updateSubElementList() override;
    const QStyleOptionMenuItem *m_menuItemOption = nullptr;
};
