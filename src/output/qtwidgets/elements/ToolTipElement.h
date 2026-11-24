// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class ToolTipElement : public AbstractElement
{
    Q_OBJECT

public:
    ToolTipElement(const UnionStyle *style, const QWidget *widget);

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;
    qreal pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

private:
    Union::IdentifierVariantHash elementAttributes() const override;
    Union::IdentifierList elementHints() const override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    void layout() override;
    const QStyleOptionFrame *m_frameOption = nullptr;
};

}
}
