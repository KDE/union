// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{
class HeaderViewElement : public AbstractElement
{
    Q_OBJECT

public:
    HeaderViewElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;
    // void layout() override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;
    // void drawText(QPainter *painter) const override;

    QRectF subElementRect(const QStyleOption *option, QStyle::SubElement element) const override;
    QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const override;
    std::optional<qreal> pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

private:
    // Union::IdentifierList elementHints() const override;
    // void updateSubElementList() override;
    // QString fallbackElementName(const QStyleOption *opt) const override;
    // const QStyleOptionHeader *m_headerOption = nullptr;
    // QIcon sortIndicator();
    bool m_isHorizontal;
};

}
}
