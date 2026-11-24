// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class ProgressBarElement;

class ProgressBarElement : public AbstractElement
{
    Q_OBJECT

public:
    ProgressBarElement(const UnionStyle *style, const QWidget *widget);

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;
    void drawText(QPainter *painter) const override;

    QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const override;
    QRectF subElementRect(const QStyleOption *option, QStyle::SubElement element) const override;
    qreal pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

    Union::IdentifierList elementHints() const override;

private:
    void updateSubElementList() override;
    void drawBackground(QPainter *painter) const override;
    void drawIndicator(QPainter *painter) const override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    void drawChunk(QPainter *painter) const;
    const QStyleOptionProgressBar *m_progressBarOption = nullptr;
    int chunkWidth() const;
    bool m_isHorizontal;
    Union::ElementList m_contentElementList;
    Union::StylePropertyGroupPtr m_contentProperties = nullptr;
};
}
}
