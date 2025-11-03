// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "Positioner.h"

#include "PositionerLayout.h"
#include "QuickStyle.h"

#include "qtquick_logging.h"

using namespace Union;

Positioner::Positioner(QObject *parent)
    : QObject(parent)
{
    m_layout = new PositionerLayout(qobject_cast<QQuickItem *>(parent));
    connect(m_layout, &PositionerLayout::implicitSizeChanged, this, &Positioner::implicitWidthChanged);
    connect(m_layout, &PositionerLayout::implicitSizeChanged, this, &Positioner::implicitHeightChanged);
    connect(m_layout, &PositionerLayout::paddingChanged, this, &Positioner::paddingChanged);
}

Positioner::~Positioner() = default;

QList<QQuickItem *> Positioner::positionItems() const
{
    return m_positionItems;
}

void Positioner::setPositionItems(const ItemList &newItems)
{
    if (newItems == m_positionItems) {
        return;
    }

    auto [addedItems, removedItems] = findPositionedItemChanges(m_positionItems, newItems);

    for (auto item : std::as_const(addedItems)) {
        auto positionedItemAttached = qobject_cast<PositionedItem *>(qmlAttachedPropertiesObject<PositionedItem>(item, false));
        if (positionedItemAttached && positionedItemAttached->positionChildren()) {
            connect(item, &QQuickItem::childrenChanged, this, [this, item]() {
                auto itr = std::find_if(m_positionChildrenItems.begin(), m_positionChildrenItems.end(), [item](const auto &watcher) {
                    return watcher.item == item;
                });

                if (itr == m_positionChildrenItems.end()) {
                    return;
                }

                const auto [addedItems, removedItems] = findPositionedItemChanges(itr->children, item->childItems());
                for (auto item : addedItems) {
                    m_layout->addItem(item);
                }

                for (auto item : removedItems) {
                    m_layout->removeItem(item);
                }
            });

            ChildrenWatcher watcher;
            watcher.item = item;
            watcher.children = item->childItems();

            const auto childItems = item->childItems();
            for (auto childItem : childItems) {
                m_layout->addItem(childItem);
            }

            m_positionChildrenItems.append(watcher);
        } else {
            m_layout->addItem(item);
        }
    }

    for (auto item : std::as_const(removedItems)) {
        auto itr = std::find_if(m_positionChildrenItems.begin(), m_positionChildrenItems.end(), [item](const auto &watcher) {
            return watcher.item == item;
        });

        if (itr != m_positionChildrenItems.end()) {
            const auto childItems = itr->children;
            for (auto childItem : childItems) {
                m_layout->removeItem(childItem);
            }
            m_positionChildrenItems.erase(itr);
        } else {
            m_layout->removeItem(item);
        }
    }

    m_positionItems = newItems;
    Q_EMIT positionItemsChanged();
}

qreal Positioner::implicitWidth() const
{
    return m_layout->implicitSize().width();
}

qreal Positioner::implicitHeight() const
{
    return m_layout->implicitSize().height();
}

Sizes Positioner::padding() const
{
    return m_layout->padding();
}

std::pair<Positioner::ItemList, Positioner::ItemList> Positioner::findPositionedItemChanges(const ItemList &currentItems, const ItemList &newItems)
{
    ItemList removedItems = currentItems;
    ItemList addedItems;

    for (auto item : newItems) {
        if (!item) {
            continue;
        }

        if (removedItems.contains(item)) {
            removedItems.removeAll(item);
        } else {
            addedItems.append(item);
        }
    }

    return std::make_pair(addedItems, removedItems);
}

Positioner *Positioner::qmlAttachedProperties(QObject *parent)
{
    return new Positioner(parent);
}

bool PositionedItem::positionChildren() const
{
    return m_positionChildren;
}

void PositionedItem::setPositionChildren(bool position)
{
    if (position == m_positionChildren) {
        return;
    }

    m_positionChildren = position;
}

PositionerSource::Source PositionedItem::source() const
{
    return m_source;
}

void PositionedItem::setSource(PositionerSource::Source newSource)
{
    m_source = newSource;
}

Union::Properties::Alignment PositionedItem::horizontalAlignment() const
{
    return m_horizontalAlignment;
}

void PositionedItem::setHorizontalAlignment(Union::Properties::Alignment newAlignment)
{
    if (newAlignment == m_horizontalAlignment) {
        return;
    }

    m_horizontalAlignment = newAlignment;
    Q_EMIT horizontalAlignmentChanged();
}

void PositionedItem::resetHorizontalAlignment()
{
    setHorizontalAlignment(Union::Properties::Alignment::Unspecified);
}

Union::Properties::Alignment PositionedItem::verticalAlignment() const
{
    return m_verticalAlignment;
}

void PositionedItem::setVerticalAlignment(Union::Properties::Alignment newAlignment)
{
    if (newAlignment == m_verticalAlignment) {
        return;
    }

    m_verticalAlignment = newAlignment;
    Q_EMIT verticalAlignmentChanged();
}

void PositionedItem::resetVerticalAlignment()
{
    setVerticalAlignment(Union::Properties::Alignment::Unspecified);
}

PositionedItem *PositionedItem::qmlAttachedProperties(QObject *parent)
{
    return new PositionedItem(parent);
}
