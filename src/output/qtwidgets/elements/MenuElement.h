// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "ShadowRenderer.h"

namespace Union
{
namespace Widgets
{

class MenuElement : public AbstractElement
{
    Q_OBJECT

public:
    MenuElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;
    QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const override;
    std::optional<qreal> pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

private:
    void updateSubElementList() override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    std::shared_ptr<ShadowRenderer> m_shadowRenderer;
    QWidget *m_shadowWidget;
};
}
}
