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
class MenuItemElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ControlElement ,QStyle::CE_MenuItem> : public TypeHelper<MenuItemElement, QStyleOptionMenuItem>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_MenuItem> : public TypeHelper<MenuItemElement, QStyleOptionMenuItem>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_MenuButtonIndicator> : public TypeHelper<MenuItemElement, QStyleOptionMenuItem>{};
/* clang-format on */
}
}

class MenuItemElement : public AbstractElement
{
    Q_OBJECT

public:
    MenuItemElement(const QStyleOptionMenuItem *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~MenuItemElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    QStringList elementHints() const override;
    void drawBackground(QPainter *painter) const override;
    void drawIndicator(QPainter *painter) const override;
    void drawText(QPainter *painter) const override;
    void updateSubElementList() override;
    QRectF adjustedRect() const;
    Union::Element::States elementStates() const override;
    const QStyleOptionMenuItem *m_menuItemOption = nullptr;
    Union::ElementList m_indicatorElementList;
    Union::Properties::StylePropertyGroup *m_indicatorProperties;
    Union::ElementList m_checkElementList;
    Union::Properties::StylePropertyGroup *m_checkProperties;
    bool m_isSeparator;
    bool m_hasSubMenu;
    bool m_hasCheckBox;
    bool m_hasRadioButton;
    QString m_shortcutText;
    qreal m_menuHMargin;
    qreal m_menuVMargin;
};
