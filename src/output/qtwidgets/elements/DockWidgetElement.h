// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "IndicatorSubElement.h"

namespace Union
{
namespace Widgets
{

class DockWidgetElement : public AbstractElement
{
    Q_OBJECT

public:
    DockWidgetElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;
    // void layout() override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;

    QRectF subElementRect(const QStyleOption *option, QStyle::SubElement subElement) const override;
    qreal pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;
    QIcon standardPixmap(const QStyleOption *option, QStyle::StandardPixmap pixmap) const override;

private:
    // void updateSubElementList() override;
    // QString fallbackElementName(const QStyleOption *opt) const override;
    // const QStyleOptionDockWidget *m_dockWidgetOption = nullptr;

    IndicatorSubElement *m_closeIndicator = nullptr;
    IndicatorSubElement *m_floatIndicator = nullptr;
};
}
}
