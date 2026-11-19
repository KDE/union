// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class TextSubElement : public AbstractElement
{
    Q_OBJECT
public:
    TextSubElement(const UnionStyle *style, QWidget *widget);

    QString text() const;
    void setText(const QString &text);

    void update(const QStyleOption *option) override;

    void draw(QPainter *painter, DrawVariant what) const override;

private:
    QString m_text;
    bool m_enabled = true;
    QColor m_color;
    Qt::LayoutDirection m_direction = Qt::LayoutDirection::LayoutDirectionAuto;
};
}
}
