// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{
class TabElement;

class TabElement : public AbstractElement
{
    Q_OBJECT

public:
    TabElement(const UnionStyle *style, const QWidget *widget = nullptr);

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    void drawText(QPainter *painter) const override;

    QRectF subElementRect(QStyle::SubElement element) const override;
    QSizeF contentsSize(QSizeF contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    Union::Element::States elementStates() const override;
    Union::IdentifierVariantHash elementAttributes() const override;
    Union::IdentifierList elementHints() const override;
    void updateSubElementList() override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    const QStyleOptionTab *m_tabOption = nullptr;
    bool m_isVertical;
    bool m_isStatic;
    QTabBar::Shape m_shape;
    QSize m_leftButtonSize;
    QSize m_rightButtonSize;
    int hSpace() const;
    int vSpace() const;
};

}
}
