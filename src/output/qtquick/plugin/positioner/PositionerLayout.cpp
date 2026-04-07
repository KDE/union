// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PositionerLayout.h"

#include <QQuickWindow>

#include "Layout.h"
#include "Positioner.h"
#include "QuickStyle.h"

#include "qtquick_logging.h"

using namespace Union::Properties;
using namespace Union::Quick;

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

    Layout layout;

    // LayoutContainer itemRelative;
    // LayoutContainer contentRelative;
    // LayoutContainer backgroundRelative;

    debug("Performing layout for positioner of", parentItem());

    for (const auto &item : std::as_const(m_items)) {
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

        auto properties = query->properties();
        auto layoutProperties = properties->layout();

        Union::Properties::AlignmentProperty *alignment;
        switch (source) {
        case PositionerSource::Source::Layout:
            alignment = layoutProperties ? layoutProperties->alignment() : nullptr;
            break;
        case PositionerSource::Source::Icon:
            alignment = properties->icon() ? properties->icon()->alignment() : nullptr;
            break;
        case PositionerSource::Source::Text:
            alignment = properties->text() ? properties->text()->alignment() : nullptr;
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

        if (source == PositionerSource::Source::Layout && layoutProperties && layoutProperties->margins()) {
            auto margins = layoutProperties->margins()->toMargins();
            layoutItem.margins = QMarginsF(margins.left(), margins.top(), margins.right(), margins.bottom());
        }

        LayoutContainer *container = &(layout.itemContainer);
        switch (alignment->container().value_or(AlignmentContainer::Item)) {
        case AlignmentContainer::Content:
            container = &(layout.contentContainer);
            break;
        case AlignmentContainer::Background:
            container = &(layout.backgroundContainer);
            break;
        case AlignmentContainer::Item:
            break;
        }

        bool stackCenter = verticalAlignment == Union::Properties::Alignment::StackCenter;
        bool stackFill = verticalAlignment == Union::Properties::Alignment::StackFill;

        switch (horizontalAlignment) {
        case Union::Properties::Alignment::Unspecified:
        case Union::Properties::Alignment::StackCenter:
        case Union::Properties::Alignment::StackFill:
            qCWarning(UNION_QTQUICK) << "Alignment" << horizontalAlignment << "not supported for horizontal alignment, item" << item
                                     << "will use Start alignment";
            [[fallthrough]];
        case Union::Properties::Alignment::Start:
            container->start.items.append(layoutItem);
            container->start.stackCenter = container->start.stackCenter || stackCenter;
            container->start.stackFill = container->start.stackFill || stackFill;
            break;
        case Union::Properties::Alignment::Center:
            container->center.items.append(layoutItem);
            container->center.stackCenter = container->center.stackCenter || stackCenter;
            container->center.stackFill = container->center.stackFill || stackFill;
            break;
        case Union::Properties::Alignment::End:
            container->end.items.append(layoutItem);
            container->end.stackCenter = container->end.stackCenter || stackCenter;
            container->end.stackFill = container->end.stackFill || stackFill;
            break;
        case Union::Properties::Alignment::Fill:
            container->fill.items.append(layoutItem);
            container->fill.stackCenter = container->fill.stackCenter || stackCenter;
            container->fill.stackFill = container->fill.stackFill || stackFill;
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
    if (!query || !query->properties()) {
        polish();
        return;
    }

    auto properties = query->properties();
    layout.size = parentItem()->size();
    layout.spacing = properties->layout() ? properties->layout()->spacing().value_or(0.0) : 0.0;
    layout.inset = properties->layout() && properties->layout()->inset() ? properties->layout()->inset()->toMargins() : QMarginsF{};
    layout.padding = properties->layout() && properties->layout()->padding() ? properties->layout()->padding()->toMargins() : QMarginsF{};
    layout.layout();

    if (layout.implicitSize != m_implicitSize) {
        debug("  New implcit size is", layout.implicitSize);
        m_implicitSize = layout.implicitSize;
        Q_EMIT implicitSizeChanged();
    }

    auto newPadding = Sizes(layout.itemPadding);
    if (newPadding != m_padding) {
        debug("  Updating padding to", layout.itemPadding.toMargins());
        m_padding = newPadding;
        Q_EMIT paddingChanged();
    }

    layout.positionItems(parentItem());

    m_layouting = false;

    Q_EMIT layoutFinished();

    if (m_requeuePolish) {
        markDirty();
        m_requeuePolish = false;
    }
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

#include "moc_PositionerLayout.cpp"
