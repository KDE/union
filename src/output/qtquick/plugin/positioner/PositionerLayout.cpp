// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PositionerLayout.h"

#include <QQmlProperty>
#include <QQuickWindow>

#include "Layout.h"
#include "Positioner.h"
#include "QuickStyle.h"

#include "qtquick_logging.h"

using namespace Union::Properties;
using namespace Union::Quick;

using namespace Qt::StringLiterals;

class PositionerLayout::Private
{
public:
    QSet<QQuickItem *> items;
    QSizeF implicitSize;
    QSizeF parentSize;

    Qt::LayoutDirection layoutDirection = Qt::LayoutDirectionAuto;

    bool layoutDirty : 1 = true;
    bool layouting : 1 = false;
    bool requeuePolish : 1 = false;

    bool paddingValid : 1 = false;
    bool insetValid : 1 = false;

    QQmlProperty spacingProperty;

    QQmlProperty leftPaddingProperty;
    QQmlProperty rightPaddingProperty;
    QQmlProperty topPaddingProperty;
    QQmlProperty bottomPaddingProperty;

    QQmlProperty leftInsetProperty;
    QQmlProperty rightInsetProperty;
    QQmlProperty topInsetProperty;
    QQmlProperty bottomInsetProperty;

    inline static const Union::Properties::LayoutProperty EmptyLayoutGroup;
};

PositionerLayout::PositionerLayout(QQuickItem *parentItem)
    : QQuickItem(parentItem)
    , d(std::make_unique<Private>())
{
    if (parentItem) {
        connect(parentItem, &QQuickItem::widthChanged, this, &PositionerLayout::onParentSizeChanged);
        connect(parentItem, &QQuickItem::heightChanged, this, &PositionerLayout::onParentSizeChanged);
        parentItem->installEventFilter(this);

        d->spacingProperty = QQmlProperty(parentItem, u"spacing"_s);
        d->spacingProperty.connectNotifySignal(this, SLOT(markDirty()));

        d->leftPaddingProperty = QQmlProperty(parentItem, u"leftPadding"_s);
        d->leftPaddingProperty.connectNotifySignal(this, SLOT(markDirty()));
        d->rightPaddingProperty = QQmlProperty(parentItem, u"rightPadding"_s);
        d->rightPaddingProperty.connectNotifySignal(this, SLOT(markDirty()));
        d->topPaddingProperty = QQmlProperty(parentItem, u"topPadding"_s);
        d->topPaddingProperty.connectNotifySignal(this, SLOT(markDirty()));
        d->bottomPaddingProperty = QQmlProperty(parentItem, u"bottomPadding"_s);
        d->bottomPaddingProperty.connectNotifySignal(this, SLOT(markDirty()));

        const auto paddingProperties = {&d->leftPaddingProperty, &d->rightPaddingProperty, &d->topPaddingProperty, &d->bottomPaddingProperty};
        d->paddingValid = std::ranges::any_of(paddingProperties, std::bind_front(std::equal_to{}, true), &QQmlProperty::isValid);

        d->leftInsetProperty = QQmlProperty(parentItem, u"leftInset"_s);
        d->leftInsetProperty.connectNotifySignal(this, SLOT(markDirty()));
        d->rightInsetProperty = QQmlProperty(parentItem, u"rightInset"_s);
        d->rightInsetProperty.connectNotifySignal(this, SLOT(markDirty()));
        d->topInsetProperty = QQmlProperty(parentItem, u"topInset"_s);
        d->topInsetProperty.connectNotifySignal(this, SLOT(markDirty()));
        d->bottomInsetProperty = QQmlProperty(parentItem, u"bottomInset"_s);
        d->bottomInsetProperty.connectNotifySignal(this, SLOT(markDirty()));

        const auto insetProperties = {&d->leftInsetProperty, &d->rightInsetProperty, &d->topInsetProperty, &d->bottomInsetProperty};
        d->insetValid = std::ranges::any_of(insetProperties, std::bind_front(std::equal_to{}, true), &QQmlProperty::isValid);
    }
    polish();
}

void PositionerLayout::markDirty()
{
    if (!d->layouting) {
        d->layoutDirty = true;
        polish();
    } else {
        d->requeuePolish = true;
    }
}

void PositionerLayout::addItem(QQuickItem *item)
{
    connect(item, &QQuickItem::implicitWidthChanged, this, &PositionerLayout::markDirty);
    connect(item, &QQuickItem::implicitHeightChanged, this, &PositionerLayout::markDirty);
    connect(item, &QQuickItem::visibleChanged, this, &PositionerLayout::markDirty);
    item->installEventFilter(this);

    d->items.insert(item);

    debug("Add item to layout", item);

    markDirty();
}

void PositionerLayout::removeItem(QQuickItem *item)
{
    auto itr = std::find(d->items.begin(), d->items.end(), item);
    if (itr == d->items.end()) {
        return;
    }

    (*itr)->disconnect(this);
    (*itr)->removeEventFilter(this);

    debug("Remove item from layout", *itr);
    d->items.erase(itr);

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

Qt::LayoutDirection PositionerLayout::layoutDirection() const
{
    return d->layoutDirection;
}

void PositionerLayout::setLayoutDirection(Qt::LayoutDirection direction)
{
    if (direction == d->layoutDirection) {
        return;
    }

    d->layoutDirection = direction;
    markDirty();
    Q_EMIT layoutDirectionChanged();
}

QSizeF PositionerLayout::implicitSize() const
{
    return d->implicitSize;
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
    if (!d->layoutDirty) {
        return;
    }

    if (!parentItem()->isVisible()) {
        return;
    }

    d->layouting = true;
    d->layoutDirty = false;

    Layout layout;

    debug("Performing layout for positioner of", parentItem());

    for (const auto &item : std::as_const(d->items)) {
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

    const auto properties = query->properties();
    const auto &layoutGroup = properties->layout() ? *(properties->layout()) : Private::EmptyLayoutGroup;

    layout.size = containerItem->size();

    layout.spacing = d->spacingProperty.isValid() ? d->spacingProperty.read().toReal() : layoutGroup.spacing().value_or(0.0);

    if (d->paddingValid) {
        layout.padding = QMarginsF{d->leftPaddingProperty.read().toReal(),
                                   d->topPaddingProperty.read().toReal(),
                                   d->rightPaddingProperty.read().toReal(),
                                   d->bottomPaddingProperty.read().toReal()};
    } else {
        layout.padding = layoutGroup.padding() ? layoutGroup.padding()->toMargins() : QMarginsF{};
    }

    if (d->insetValid) {
        layout.inset = QMarginsF{d->leftInsetProperty.read().toReal(),
                                 d->topInsetProperty.read().toReal(),
                                 d->rightInsetProperty.read().toReal(),
                                 d->bottomInsetProperty.read().toReal()};
    } else {
        layout.inset = layoutGroup.inset() ? layoutGroup.inset()->toMargins() : QMarginsF{};
    }

    layout.layout();

    if (layout.implicitSize != d->implicitSize) {
        debug("  New implcit size is", layout.implicitSize);
        d->implicitSize = layout.implicitSize;
        Q_EMIT implicitSizeChanged();
    }

    layout.positionItems(parentItem(), d->layoutDirection == Qt::LayoutDirectionAuto ? qApp->layoutDirection() : d->layoutDirection);

    d->layouting = false;

    Q_EMIT layoutFinished();

    if (d->requeuePolish) {
        markDirty();
        d->requeuePolish = false;
    }
}

void PositionerLayout::onParentSizeChanged()
{
    auto newSize = parentItem()->boundingRect().size();
    if (newSize == d->parentSize) {
        return;
    }

    d->parentSize = newSize;
    markDirty();
}

#include "moc_PositionerLayout.cpp"
