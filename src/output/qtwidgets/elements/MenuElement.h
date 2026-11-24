// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "ShadowRenderer.h"

namespace Union
{
namespace Widgets
{

class MenuElement;

class MenuElement : public AbstractElement
{
    Q_OBJECT

public:
    MenuElement(const UnionStyle *style, const QWidget *widget = nullptr);
    ~MenuElement() override;

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    QSizeF contentsSize(QSizeF contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    void updateSubElementList() override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    std::shared_ptr<ShadowRenderer> m_shadowRenderer;
    QWidget *m_shadowWidget;
};
}
}
