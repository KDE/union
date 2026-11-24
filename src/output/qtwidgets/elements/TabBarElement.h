// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class TabBarElement;

class TabBarElement : public AbstractElement
{
    Q_OBJECT

public:
    TabBarElement(const UnionStyle *style, const QWidget *widget);

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;

    qreal pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;
    int styleHint(const QStyleOption *option, QStyle::StyleHint styleHint) const override;
    Union::IdentifierVariantHash elementAttributes() const override;
    Union::IdentifierList elementHints() const override;

private:
    QString fallbackElementName(const QStyleOption *opt) const override;
    void drawBackground(QPainter *painter) const override;
    const QStyleOptionTabBarBase *m_tabBarOption = nullptr;
    qreal scrollButtonWidth() const;
};
}
}
