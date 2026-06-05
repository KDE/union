// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QWidget>

#include <PropertiesTypes.h>

namespace Union
{
namespace Widgets
{
struct LayoutItem {
    QPointF position = QPointF{0.0, 0.0};
    QSizeF size = QSizeF{0.0, 0.0};
    QSizeF implicitSize = QSizeF{0.0, 0.0};
    QSizeF minimumSize = QSizeF{0.0, 0.0};
    Union::Properties::Alignment verticalAlignment = Union::Properties::Alignment::Unspecified;
    int order = 0;
    QMarginsF margins;
    QWidget *item = nullptr;

    void setItemPosition(QWidget *parent, Qt::LayoutDirection direction)
    {
        auto positionInParent = position;
        if (direction == Qt::RightToLeft) {
            auto parentSize = parent->rect().size();
            positionInParent = QPointF{parentSize.width() - size.width() - position.x(), position.y()};
        }

        auto mapped = parent->mapFromParent(positionInParent);
        item->rect().setX(std::round(mapped.x()));
        item->rect().setY(std::round(mapped.y()));
        item->rect().setSize(QSize{static_cast<int>(std::ceil(size.width())), static_cast<int>(std::ceil(size.height()))});
    }
};

}
}
