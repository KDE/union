// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class IndicatorElement;

// This is a kitchen-sink element class to draw any various indicators

class IndicatorElement : public AbstractElement
{
    Q_OBJECT

public:
    IndicatorElement(const UnionStyle *style, const QWidget *widget);

    void draw(QPainter *painter, DrawEnums enums) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    void drawElement(QPainter *painter, const QString &defaultIconName, const QStringList &targetHierarchy) const;

    int m_listViewIconSize = 0;
    int m_smallIconSize = 0;
    int m_iconViewIconSize = 0;
    int m_largeIconSize = 0;
    int m_messageBoxIconSize = 0;
    int m_textCursorWidth = 0;
};
}
}
