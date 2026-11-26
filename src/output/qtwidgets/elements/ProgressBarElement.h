// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "IndicatorSubElement.h"

namespace Union
{
namespace Widgets
{

class ProgressBarElement : public AbstractElement
{
    Q_OBJECT

public:
    ProgressBarElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;
    // void layout() override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;
    // void drawText(QPainter *painter) const override;

    QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const override;
    std::optional<QRectF> subElementRect(const QStyleOption *option, QStyle::SubElement element) const override;
    std::optional<qreal> pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

protected:
    void drawBackground(QPainter *painter, UnionPainter::Parts parts = UnionPainter::Parts::All) const override;

private:
    QRectF filledIndicatorRect(const QStyleOptionProgressBar *progressOption);
    // void updateSubElementList() override;
    // void drawIndicator(QPainter *painter) const override;
    // QString fallbackElementName(const QStyleOption *opt) const override;
    // void drawChunk(QPainter *painter) const;
    // const QStyleOptionProgressBar *m_progressBarOption = nullptr;
    // int chunkWidth() const;
    bool m_isHorizontal = true;
    IndicatorSubElement *m_filledIndicator = nullptr;
    // Union::ElementList m_contentElementList;
    // Union::StylePropertyGroupPtr m_contentProperties = nullptr;
};
}
}
