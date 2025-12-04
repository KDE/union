// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PositionerLayout.h"

#include <QQuickWindow>

#include "Positioner.h"
#include "QuickStyle.h"

#include "qtquick_logging.h"

using namespace Union::Properties;

inline qreal spacedSize(const std::initializer_list<qreal> &sizes, qreal spacing)
{
    Q_ASSERT(sizes.size() > 0);

    qreal result = *sizes.begin();
    if (sizes.size() > 1) {
        for (auto itr = sizes.begin() + 1; itr != sizes.end(); ++itr) {
            if (!qFuzzyIsNull(*itr)) {
                if (result > 0) {
                    result += spacing;
                }
                result += *itr;
            }
        }
    }
    return result;
}

inline qreal spacedSize(qreal input, qreal spacing)
{
    if (!qFuzzyIsNull(input)) {
        return input + spacing;
    }
    return input;
}

PositionerLayout::PositionerLayout(QQuickItem *parentItem)
    : QQuickItem(parentItem)
{
    if (parentItem) {
        connect(parentItem, &QQuickItem::widthChanged, this, &PositionerLayout::onParentSizeChanged);
        connect(parentItem, &QQuickItem::heightChanged, this, &PositionerLayout::onParentSizeChanged);
        parentItem->installEventFilter(this);
    }
    polish();
}

void PositionerLayout::markDirty()
{
    if (!m_layouting) {
        m_layoutDirty = true;
        polish();
    } else {
        m_requeuePolish = true;
    }
}

void PositionerLayout::addItem(QQuickItem *item)
{
    connect(item, &QQuickItem::implicitWidthChanged, this, &PositionerLayout::markDirty);
    connect(item, &QQuickItem::implicitHeightChanged, this, &PositionerLayout::markDirty);
    connect(item, &QQuickItem::visibleChanged, this, &PositionerLayout::markDirty);
    item->installEventFilter(this);

    m_items.insert(item);

    debug("Add item to layout", item);

    markDirty();
}

void PositionerLayout::removeItem(QQuickItem *item)
{
    auto itr = std::find(m_items.begin(), m_items.end(), item);
    if (itr == m_items.end()) {
        return;
    }

    (*itr)->disconnect(this);
    (*itr)->removeEventFilter(this);

    debug("Remove item from layout", *itr);
    m_items.erase(itr);

    markDirty();
}

bool PositionerLayout::isDebugEnabled() const
{
    return m_debugEnabled;
}

void PositionerLayout::setDebugEnabled(bool newDebug)
{
    m_debugEnabled = newDebug;
}

QSizeF PositionerLayout::implicitSize() const
{
    return m_implicitSize;
}

Sizes PositionerLayout::padding() const
{
    return m_padding;
}

bool PositionerLayout::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QuickStyleUpdatedEvent::s_type) {
        markDirty();
        return false;
    }

    return QQuickItem::eventFilter(target, event);
}

void PositionerLayout::updatePolish()
{
    if (!m_layoutDirty) {
        return;
    }

    if (!parentItem()->isVisible()) {
        return;
    }

    m_layouting = true;
    m_layoutDirty = false;

    LayoutContainer itemRelative;
    LayoutContainer contentRelative;
    LayoutContainer backgroundRelative;

    debug("Performing layout for positioner of", parentItem());

    for (auto &item : m_items) {
        auto source = PositionerSource::Source::Layout;

        if (!item->isVisible()) {
            debug("  Ignoring invisible item", item);
            continue;
        }

        auto positionedItemAttached = qobject_cast<PositionedItem *>(qmlAttachedPropertiesObject<PositionedItem>(item, false));
        if (positionedItemAttached) {
            source = positionedItemAttached->source();
        }

        auto query = qobject_cast<QuickStyle *>(qmlAttachedPropertiesObject<QuickStyle>(item, true))->query();
        if (!query) {
            // Apparently the item has not completed yet, abort layouting and
            // try again the next frame.
            polish();
            break;
        }

        std::optional<Union::Properties::AlignmentProperty> alignment;
        switch (source) {
        case PositionerSource::Source::Layout:
            alignment = query->properties().layout().value_or(LayoutProperty{}).alignment();
            break;
        case PositionerSource::Source::Icon:
            alignment = query->properties().icon().value_or(IconProperty{}).alignment();
            break;
        case PositionerSource::Source::Text:
            alignment = query->properties().text().value_or(TextProperty{}).alignment();
            break;
        }

        if (!alignment) {
            debug("  No alignment found for item", item);
            continue;
        }

        auto horizontalAlignment = alignment->horizontal().value_or(Union::Properties::Alignment::Unspecified);
        auto verticalAlignment = alignment->vertical().value_or(Union::Properties::Alignment::Unspecified);

        if (positionedItemAttached) {
            if (positionedItemAttached->horizontalAlignment() != Union::Properties::Alignment::Unspecified) {
                horizontalAlignment = positionedItemAttached->horizontalAlignment();
            }

            if (positionedItemAttached->verticalAlignment() != Union::Properties::Alignment::Unspecified) {
                verticalAlignment = positionedItemAttached->verticalAlignment();
            }
        }

        LayoutItem layoutItem{
            .implicitSize = QSizeF{item->implicitWidth(), item->implicitHeight()},
            .verticalAlignment = verticalAlignment,
            .order = alignment->order().value_or(0),
            .margins = QMarginsF{},
            .item = item,
        };

        if (source == PositionerSource::Source::Layout) {
            auto margins = query->properties().layout().value_or(LayoutProperty{}).margins().value_or(SizeProperty{});
            layoutItem.margins =
                QMarginsF(margins.left().value_or(0.0), margins.top().value_or(0.0), margins.right().value_or(0.0), margins.bottom().value_or(0.0));
        }

        LayoutContainer *container = &itemRelative;
        switch (alignment->container().value_or(AlignmentContainer::Item)) {
        case AlignmentContainer::Content:
            container = &contentRelative;
            break;
        case AlignmentContainer::Background:
            container = &backgroundRelative;
            break;
        case AlignmentContainer::Item:
            break;
        }

        bool stacked = verticalAlignment == Union::Properties::Alignment::Stack;

        switch (horizontalAlignment) {
        case Union::Properties::Alignment::Unspecified:
        case Union::Properties::Alignment::Stack:
            qCWarning(UNION_QTQUICK) << "Alignment" << horizontalAlignment << "not supported for horizontal alignment, item" << item
                                     << "will use Start alignment";
            [[fallthrough]];
        case Union::Properties::Alignment::Start:
            container->start.items.append(layoutItem);
            container->start.stacked = container->start.stacked || stacked;
            break;
        case Union::Properties::Alignment::Center:
            container->center.items.append(layoutItem);
            container->center.stacked = container->center.stacked || stacked;
            break;
        case Union::Properties::Alignment::End:
            container->end.items.append(layoutItem);
            container->end.stacked = container->end.stacked || stacked;
            break;
        case Union::Properties::Alignment::Fill:
            container->fill.items.append(layoutItem);
            container->fill.stacked = container->fill.stacked || stacked;
            break;
        }

        debug("  Layout item", item);
        debug("    Implicit Size:", layoutItem.implicitSize);
        debug("    Container:", alignment->container());
        debug("    Alignment: (h)", horizontalAlignment, "(v)", verticalAlignment);
        debug("    Order:", layoutItem.order);
    }

    const auto containerItem = parentItem();
    const auto query = qobject_cast<QuickStyle *>(qmlAttachedPropertiesObject<QuickStyle>(containerItem, true))->query();
    if (!query) {
        polish();
        return;
    }
    qreal spacing = query->properties().layout().value_or(LayoutProperty{}).spacing().value_or(0.0);

    auto sort = [](auto &container) {
        std::stable_sort(container.begin(), container.end(), [](auto first, auto second) {
            return first.order < second.order;
        });
    };

    for (auto container : {&itemRelative, &contentRelative, &backgroundRelative}) {
        container->spacing = spacing;
        container->start.spacing = spacing;
        container->center.spacing = spacing;
        container->end.spacing = spacing;
        container->fill.spacing = spacing;

        sort(container->start.items);
        sort(container->center.items);
        sort(container->end.items);
        sort(container->fill.items);
    }

    itemRelative.size = QSizeF{containerItem->width(), containerItem->height()};
    layoutContainer(itemRelative);

    auto remainingPosition = QPointF{spacedSize(itemRelative.start.size.width(), spacing), 0.0};
    auto remainingSize = itemRelative.size - QSizeF{spacedSize({itemRelative.start.size.width(), itemRelative.end.size.width()}, spacing), 0.0};

    auto inset = query->properties().layout().value_or(LayoutProperty{}).inset().value_or(SizeProperty{}).toMargins();
    backgroundRelative.position = remainingPosition + QPointF{inset.left(), inset.top()};
    backgroundRelative.size = remainingSize - QSizeF{inset.left() + inset.right(), inset.top() + inset.bottom()};
    layoutContainer(backgroundRelative);

    auto padding = query->properties().layout().value_or(LayoutProperty{}).padding().value_or(SizeProperty{}).toMargins();
    contentRelative.position = remainingPosition + QPointF{padding.left(), padding.top()};
    contentRelative.size = remainingSize - QSizeF{padding.left() + padding.right(), padding.top() + padding.bottom()};
    layoutContainer(contentRelative);

    for (auto container : {&itemRelative, &backgroundRelative, &contentRelative}) {
        for (auto bucket : {&container->start, &container->end, &container->center, &container->fill}) {
            auto position = bucket->position;
            for (auto item : bucket->items) {
                auto itemPosition = position + item.position;
                auto mapped = parentItem()->mapToItem(item.item->parentItem(), itemPosition);
                item.item->setX(mapped.x());
                item.item->setY(mapped.y());
                item.item->setWidth(item.size.width());
                item.item->setHeight(item.size.height());

                debug("  Item", item.item, "positioned at", itemPosition, mapped);
            }
        }
    }

    auto implicitCenterWidth = std::max(backgroundRelative.implicitSize.width() + inset.left() + inset.right(),
                                        contentRelative.implicitSize.width() + padding.left() + padding.right());
    auto implicitWidth = spacedSize({itemRelative.start.implicitSize.width(), implicitCenterWidth, itemRelative.end.implicitSize.width()}, spacing);
    auto implicitHeight = std::max({itemRelative.implicitSize.height(),
                                    backgroundRelative.implicitSize.height() + inset.top() + inset.bottom(),
                                    contentRelative.implicitSize.height() + padding.top() + padding.bottom()});
    auto newImplicitSize = QSizeF{std::round(implicitWidth), std::round(implicitHeight)};
    if (newImplicitSize != m_implicitSize) {
        m_implicitSize = newImplicitSize;
        Q_EMIT implicitSizeChanged();
    }

    auto bottomRight = itemRelative.size - contentRelative.size;
    auto newPadding = Sizes(contentRelative.position.x(),
                            bottomRight.width() - contentRelative.position.x(),
                            contentRelative.position.y(),
                            bottomRight.height() - contentRelative.position.y());
    if (m_padding != newPadding) {
        m_padding = newPadding;
        Q_EMIT paddingChanged();
    }

    m_layouting = false;

    Q_EMIT layoutFinished();

    if (m_requeuePolish) {
        markDirty();
        m_requeuePolish = false;
    }
}

void PositionerLayout::layoutContainer(LayoutContainer &container)
{
    for (auto bucket : {&container.start, &container.end, &container.center, &container.fill}) {
        layoutBucket(*bucket);
        bucket->size = QSizeF{bucket->implicitSize.width(), container.size.height()};
    }

    container.start.position = container.position;
    container.end.position = QPointF{container.position.x() + container.size.width() - container.end.implicitSize.width(), container.position.y()};

    QPointF remainingPosition = container.position + QPointF{spacedSize(container.start.implicitSize.width(), container.spacing), 0.0};
    QSizeF remainingSize = container.size - QSizeF{spacedSize({container.start.size.width(), container.end.size.width()}, container.spacing), 0.0};

    container.fill.position = remainingPosition;
    container.fill.size = remainingSize;

    container.center.position = remainingPosition + QPointF{(remainingSize.width() - container.center.size.width()) / 2, 0.0};

    auto maxImplicitCenter = QSizeF{std::max(container.center.implicitSize.width(), container.fill.implicitSize.width()), //
                                    std::max(container.center.implicitSize.height(), container.fill.implicitSize.height())};
    container.implicitSize =
        QSizeF{spacedSize({container.start.implicitSize.width(), maxImplicitCenter.width(), container.end.implicitSize.width()}, container.spacing),
               std::max({container.start.implicitSize.height(), maxImplicitCenter.height(), container.end.implicitSize.height()})};

    auto maxCenter = QSizeF{std::max(container.center.size.width(), container.fill.size.width()), //
                            std::max(container.center.size.height(), container.fill.size.height())};
    container.size = QSizeF{spacedSize({container.start.size.width(), maxCenter.width(), container.end.size.width()}, container.spacing),
                            std::max({container.start.size.height(), maxCenter.height(), container.end.size.height()})};

    for (auto bucket : {&container.start, &container.end, &container.center, &container.fill}) {
        qreal stackedY = 0.0;
        qreal stackedHeight = bucket->size.height() / bucket->items.count();
        qreal fillX = 0.0;
        qreal fillWidth = bucket->size.width() / bucket->items.count();
        for (auto &item : bucket->items) {
            item.size = item.implicitSize;

            if (bucket == &container.fill) {
                fillX += item.margins.left();
                item.position.setX(fillX);
                item.size.setWidth(fillWidth - item.margins.left() - item.margins.right());
                fillX += fillWidth;
            }

            switch (item.verticalAlignment) {
            case Union::Properties::Alignment::Unspecified:
            case Union::Properties::Alignment::Start:
                item.position.setY(item.margins.top());
                break;
            case Union::Properties::Alignment::Center:
                item.position.setY(std::round(bucket->size.height() / 2 - (item.implicitSize.height() + item.margins.top() + item.margins.bottom()) / 2));
                break;
            case Union::Properties::Alignment::End:
                item.position.setY(bucket->size.height() - item.implicitSize.height() - item.margins.bottom());
                break;
            case Union::Properties::Alignment::Fill:
                item.position.setY(item.margins.top());
                item.size.setHeight(bucket->size.height() - item.margins.top() - item.margins.bottom());
                break;
            case Union::Properties::Alignment::Stack:
                stackedY += item.margins.top();
                item.position.setY(stackedY);
                item.size.setWidth(bucket->size.width());
                item.size.setHeight(stackedHeight - item.margins.top() - item.margins.bottom());
                stackedY += stackedHeight + item.margins.bottom();
                break;
            }
        }
    }
}

void PositionerLayout::layoutBucket(LayoutBucket &bucket)
{
    qreal x = 0.0;
    qreal maxWidth = 0.0;
    qreal maxHeight = 0.0;
    qreal totalHeight = 0.0;

    for (auto &item : bucket.items) {
        if (bucket.stacked) {
            item.position.setX(0.0);
            totalHeight += item.margins.top() + item.margins.bottom() + item.implicitSize.height();
        } else {
            if (qFuzzyIsNull(item.implicitSize.width())) {
                continue;
            }

            if (x > 0.0) {
                x += bucket.spacing;
            }

            x += item.margins.left();
            item.position.setX(x);
            x += item.implicitSize.width() + item.margins.right();
        }

        maxWidth = std::max(maxWidth, item.margins.left() + item.implicitSize.width() + item.margins.right());
        maxHeight = std::max(maxHeight, item.margins.top() + item.implicitSize.height() + item.margins.bottom());
    }

    bucket.implicitSize.setWidth(std::round(std::max(bucket.stacked ? maxWidth : x, 0.0)));
    bucket.implicitSize.setHeight(std::round(std::max(bucket.stacked ? totalHeight : maxHeight, 0.0)));
}

void PositionerLayout::onParentSizeChanged()
{
    auto newSize = parentItem()->boundingRect().size();
    if (newSize == m_parentSize) {
        return;
    }

    m_parentSize = newSize;
    markDirty();
}
