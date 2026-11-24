// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class ItemViewElement;

class ItemViewElement : public AbstractElement
{
    Q_OBJECT

public:
    ItemViewElement(const UnionStyle *style, const QWidget *widget = nullptr);

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subElementRect(QStyle::SubElement element) const override;
    QSizeF contentsSize(QSizeF contentsSizeFromStyle) const override;

private:
    void drawBackground(QPainter *painter) const override;
    Union::IdentifierVariantHash elementAttributes() const override;
    Union::IdentifierList elementHints() const override;
    void drawIndicator(QPainter *painter) const override;
    void drawText(QPainter *painter) const override;
    void drawIcon(QPainter *painter) const override;
    const QStyleOptionViewItem *m_viewItemOption = nullptr;
    QString fallbackElementName(const QStyleOption *opt) const override;
    Union::ElementList m_indicatorElementList;
    Union::StylePropertyGroupPtr m_indicatorProperties = nullptr;
    qreal m_totalSpacing = 0;
};
}
}
