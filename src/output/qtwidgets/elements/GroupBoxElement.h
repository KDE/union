// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{
class GroupBoxElement : public AbstractElement
{
    Q_OBJECT

public:
    GroupBoxElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;
    QRectF subControlRect(const QStyleOption *option, QStyle::SubControl subControl) const override;
    int styleHint(const QStyleOption *option, QStyle::StyleHint styleHint) const override;
    std::optional<qreal> pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

private:
    bool m_isCheckable = false;
};
}
}
