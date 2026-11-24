// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

// This is a kitchen-sink element class to draw any various indicators
class TreeViewElement : public AbstractElement
{
    Q_OBJECT

public:
    TreeViewElement(const UnionStyle *style, const QWidget *widget);

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;
    qreal pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

private:
    Union::IdentifierList elementHints() const override;
    void drawIndicator(QPainter *painter) const override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    qreal indentation() const;
    bool m_itemIsOpen;

    Union::ElementList m_indicatorElementList;
    Union::StylePropertyGroupPtr m_indicatorProperties = nullptr;
};

}
}
