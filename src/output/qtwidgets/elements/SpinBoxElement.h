// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class SpinBoxElement : public AbstractElement
{
    Q_OBJECT

public:
    SpinBoxElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;
    qreal pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

    QRectF subControlRect(const QStyleOption *option, QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const override;

private:
    Union::IdentifierList elementHints() const override;
    void updateSubElementList() override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    void drawSpinIndicator(QPainter *painter, QStyle::PrimitiveElement primitive) const;
    const QStyleOptionSpinBox *m_spinBoxOption = nullptr;
    bool m_hasButtons;
    bool m_hasFrame;
    bool m_usesArrows;
};
}
}
