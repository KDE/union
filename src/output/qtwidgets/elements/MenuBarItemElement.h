// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class MenuBarItemElement;

class MenuBarItemElement : public AbstractElement
{
    Q_OBJECT

public:
    MenuBarItemElement(const UnionStyle *style, const QWidget *widget);

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    void layout() override;

private:
    Union::IdentifierList elementHints() const override;
    void updateSubElementList() override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    const QStyleOptionMenuItem *m_menuItemOption = nullptr;
};
}
}
