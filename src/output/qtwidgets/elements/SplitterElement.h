// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class SplitterElement;

class SplitterElement : public AbstractElement
{
    Q_OBJECT

public:
    SplitterElement(const UnionStyle *style, const QWidget *widget);

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    QString fallbackElementName(const QStyleOption *opt) const override;
};

}
}
