// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class TabCloseButtonElement;

class TabCloseButtonElement : public AbstractElement
{
    Q_OBJECT

public:
    TabCloseButtonElement(const UnionStyle *style, const QWidget *widget = nullptr);
    ~TabCloseButtonElement() override;

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;
};
}
}
