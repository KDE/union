// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QObject>
#include <QQuickItem>

#include <PropertiesTypes.h>

#include "Types.h"

namespace Union
{
namespace Quick
{

struct LayoutContainer;

/**
 * An internal class that performs the actual layout of Positioner.
 *
 * This has a list of items that will be placed according to the alignment
 * information  from the style.
 *
 * The items will be separated into three containers: Item, Background and
 * Content. These containers determine what the item will be placed relative to,
 * with Item meaning they will be placed relative to the root item, Background
 * meaning they will be relative to the background and Content meaning they will
 * be relative to the content. The background container will account for inset
 * while the Content container will account for padding.
 *
 * Within each container, the items will be further sorted into several buckets,
 * based on their horizontal alignment, one for Start, one for End, one for
 * Center and one for Fill. The items will first be laid out according to the
 * item's size hints, which determines the size of the bucket. These buckets
 * will then be positioned based on their alignment, with Start being placed at
 * the start of the control, End being placed at the end, Center being placed at
 * the center and Fill taking the available width and dividing it equally
 * between its items.
 *
 * Note that the start and end buckets reduce the space that is available for
 * the center and fill buckets. In addition, anything placed in the start and
 * end buckets of the Item container will reduce the space available for the
 * Background and Content containers.
 *
 * Note also that this is a QQuickItem purely to allow for the usage of
 * `polish()` and `updatePolish()`, for which there are no external hooks. An
 * instance of this class will be added by PositionerAttached to the item it is
 * attached to.
 */
class PositionerLayout : public QQuickItem
{
    Q_OBJECT

public:
    PositionerLayout(QQuickItem *parentItem);

    Q_SLOT void markDirty();

    void addItem(QQuickItem *item);
    void removeItem(QQuickItem *item);

    bool isDebugEnabled() const;
    void setDebugEnabled(bool newDebug);

    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection(Qt::LayoutDirection direction);
    Q_SIGNAL void layoutDirectionChanged();

    QSizeF implicitSize() const;
    Q_SIGNAL void implicitSizeChanged();

    Q_SIGNAL void layoutFinished();

protected:
    bool eventFilter(QObject *target, QEvent *event) override;
    void updatePolish() override;

private:
    void onParentSizeChanged();
    void onItemPositionChanged();

    template<typename... Args>
    inline void debug(Args &&...args)
    {
        if (m_debugEnabled) {
            (QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, "union.quick").debug() << ... << args);
        }
    }

    bool m_debugEnabled = false;

    class Private;
    const std::unique_ptr<Private> d;
};

}
}
