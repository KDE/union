// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class CheckBoxElement : public AbstractElement
{
    Q_OBJECT

public:
    CheckBoxElement(const UnionStyle *style, QWidget *widget = nullptr);

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawVariant what) const override;
    void layout() override;
    QRectF subElementRect(QStyle::SubElement element) const override;
    QSizeF contentsSize(QSizeF contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

    static AbstractElement *create(const UnionStyle *style, QWidget *widget)
    {
        return new CheckBoxElement(style, widget);
    }

private:
    // void updateSubElementList() override;
    // void drawIndicator(QPainter *painter) const override;
    // Union::IdentifierList elementHints() const override;
    // QString fallbackElementName(const QStyleOption *opt) const override;
    // const QStyleOptionButton *m_buttonOption = nullptr;
    // Union::ElementList m_indicatorElementList;
    // Union::StylePropertyGroupPtr m_indicatorProperties = nullptr;
};

}
}
