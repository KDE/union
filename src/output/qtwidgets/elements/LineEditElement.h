// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class LineEditElement : public AbstractElement
{
    Q_OBJECT

public:
    LineEditElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;
    QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const override;
    QRectF subElementRect(const QStyleOption *option, QStyle::SubElement element) const override;
    qreal pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

protected:
    void drawBackground(QPainter *painter, UnionPainter::Parts parts = UnionPainter::Parts::All) const override;

private:
    // void updateSubElementList() override;
    // Union::IdentifierVariantHash elementAttributes() const override;
    // Union::IdentifierList elementHints() const override;
    // QString fallbackElementName(const QStyleOption *opt) const override;
    // QMarginsF iconPadding() const;
    // const QStyleOptionFrame *m_frameOption = nullptr;
    bool m_hasFrame = true;
    std::optional<QBrush> m_backgroundBrush;
};
}
}
