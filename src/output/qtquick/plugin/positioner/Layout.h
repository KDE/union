// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include "../Types.h"
#include "LayoutContainer.h"
#include "LayoutHelpers.h"

namespace Union
{
namespace Quick
{

struct Layout {
    LayoutContainer itemContainer;
    LayoutContainer contentContainer;
    LayoutContainer backgroundContainer;

    QSizeF size;
    QMarginsF inset;
    QMarginsF padding;
    qreal spacing = 0.0;

    QSizeF implicitSize;
    QMarginsF itemPadding;

    void layout()
    {
        for (auto container : {&itemContainer, &contentContainer, &backgroundContainer}) {
            container->setSpacing(spacing);
            container->sort();
        }

        itemContainer.size = size;
        itemContainer.layout();

        auto remainingPosition = QPointF{itemContainer.start.size.width(), 0.0};
        auto remainingSize = itemContainer.size - QSizeF{itemContainer.start.size.width() + itemContainer.end.size.width(), 0.0};

        backgroundContainer.position = remainingPosition + QPointF{inset.left(), inset.top()};
        backgroundContainer.size = remainingSize - QSizeF{inset.left() + inset.right(), inset.top() + inset.bottom()};
        backgroundContainer.layout();

        contentContainer.position = remainingPosition + QPointF{padding.left(), padding.top()};
        contentContainer.size = remainingSize - QSizeF{padding.left() + padding.right(), padding.top() + padding.bottom()};
        contentContainer.layout();

        auto implicitCenterWidth = std::max(backgroundContainer.implicitSize.width() + inset.left() + inset.right(),
                                            contentContainer.implicitSize.width() + padding.left() + padding.right());

        auto implicitWidth = spacedSize(spacing, itemContainer.start.implicitSize.width(), implicitCenterWidth, itemContainer.end.implicitSize.width());
        auto implicitHeight = std::max({itemContainer.implicitSize.height(),
                                        backgroundContainer.implicitSize.height() + inset.top() + inset.bottom(),
                                        contentContainer.implicitSize.height() + padding.top() + padding.bottom()});

        implicitSize = QSizeF{std::ceil(implicitWidth), std::ceil(implicitHeight)};

        auto bottomRight = itemContainer.size - contentContainer.size;
        itemPadding = QMarginsF(contentContainer.position.x(),
                                bottomRight.width() - contentContainer.position.x(),
                                contentContainer.position.y(),
                                bottomRight.height() - contentContainer.position.y());
    }

    void positionItems(QQuickItem *parent)
    {
        for (auto container : {&itemContainer, &backgroundContainer, &contentContainer}) {
            container->positionItems(parent);
        }
    }
};

}
}
