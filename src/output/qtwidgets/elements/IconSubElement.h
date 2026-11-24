// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{
class IconSubElement : public AbstractElement
{
    Q_OBJECT
public:
    IconSubElement(const UnionStyle *style, QWidget *widget);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QSize size() const;
    void setSize(QSize size);

    void update(const QStyleOption *option) override;

    void draw(QPainter *painter, DrawVariant what) const override;

private:
    QIcon m_icon;
    QSize m_size;
    bool m_enabled = true;
    QColor m_color;
};
}
}
