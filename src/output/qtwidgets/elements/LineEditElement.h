// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class LineEditElement;

class LineEditElement : public AbstractElement
{
    Q_OBJECT

public:
    LineEditElement(const UnionStyle *style, const QWidget *widget = nullptr);

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    void drawBackground(QPainter *painter) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;
    QSizeF contentsSize(QSizeF contentsSizeFromStyle) const override;

    QRectF subElementRect(QStyle::SubElement element) const override;

private:
    void updateSubElementList() override;
    Union::IdentifierVariantHash elementAttributes() const override;
    Union::IdentifierList elementHints() const override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    QMarginsF iconPadding() const;
    const QStyleOptionFrame *m_frameOption = nullptr;
    bool m_hasFrame;
};
}
}
