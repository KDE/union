// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class TabWidgetElement;

class TabWidgetElement : public AbstractElement
{
    Q_OBJECT

public:
    TabWidgetElement(const UnionStyle *style, const QWidget *widget = nullptr);

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawEnums enums) const override;

private:
    Union::IdentifierVariantHash elementAttributes() const override;
    QString fallbackElementName(const QStyleOption *opt) const override;

    const QStyleOptionTabWidgetFrame *m_tabFrameOption = nullptr;

    QTabBar::Shape m_shape;
};

}
}
