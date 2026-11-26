// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include <QFrame>

#include "AbstractElement.h"
#include "ShadowRenderer.h"

namespace Union
{
namespace Widgets
{
class FrameElement : public AbstractElement
{
    Q_OBJECT

public:
    FrameElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;

    std::optional<qreal> pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;
    std::optional<QRectF> subElementRect(const QStyleOption *option, QStyle::SubElement element) const override;

    // void layout() override;
    // void drawFrame(QPainter *painter) const override;
    // These are shared between any element that uses QStyleOptionFrame
    static Union::IdentifierVariantHash frameAttributesFor(const QStyleOptionFrame *frameOption, const QWidget *widget);
    static Union::IdentifierList frameHintsFor(const QStyleOptionFrame *frameOption);

private:
    qreal frameWidth() const;

    QFrame::Shape m_frameShape = QFrame::Shape::NoFrame;
    // Union::IdentifierVariantHash elementAttributes() const override;
    // Union::IdentifierList elementHints() const override;
    // void updateSubElementList() override;
    // QString fallbackElementName(const QStyleOption *opt) const override;
    // void drawShadow() const;
    // const QStyleOptionFrame *m_frameOption = nullptr;
    // std::shared_ptr<ShadowRenderer> m_shadowRenderer;
    // QWidget *m_shadowWidget;
};
}
}
