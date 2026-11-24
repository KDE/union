// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class ButtonElement : public AbstractElement
{
    Q_OBJECT

public:
    ButtonElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;

    void draw(QPainter *painter, AbstractElement::DrawVariant what) const override;

    QRectF subElementRect(const QStyleOption *option, QStyle::SubElement element) const override;
    QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const override;
    qreal pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;
};

}
}
