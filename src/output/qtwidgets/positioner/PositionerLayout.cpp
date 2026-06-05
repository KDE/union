// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PositionerLayout.h"

#include <QWidget>

#include <properties/StylePropertyGroup.h>

#include "Layout.h"
#include "Positioner.h"

using namespace Union::Properties;
using namespace Union::Widgets;

using namespace Qt::StringLiterals;

class PositionerLayout::Private
{
public:
    QWidgetSet items;
    QSizeF implicitSize;
    QSizeF parentSize;

    Qt::LayoutDirection layoutDirection = Qt::LayoutDirectionAuto;

    bool layoutDirty : 1 = true;
    bool layouting : 1 = false;
    bool requeuePolish : 1 = false;
    bool ignorePositionChange : 1 = false;

    bool paddingValid : 1 = false;
    bool insetValid : 1 = false;

    bool mirroredProperty;
    qreal spacingProperty;
    qreal leftPaddingProperty;
    qreal rightPaddingProperty;
    qreal topPaddingProperty;
    qreal bottomPaddingProperty;
    qreal leftInsetProperty;
    qreal rightInsetProperty;
    qreal topInsetProperty;
    qreal bottomInsetProperty;

    inline static const Union::Properties::LayoutPropertyGroup EmptyLayoutGroup;
};

PositionerLayout::PositionerLayout(const Union::Properties::StylePropertyGroup *properties, const QWidget *parentWidget)
    : QWidget()
    , m_properties(properties)
    , d(std::make_unique<Private>())
{
    if (parentWidget) {
        // TODO: Use event filter instead
        // connect(parentWidget, &QWidget::widthChanged, this, &PositionerLayout::onParentSizeChanged);
        // connect(parentWidget, &QWidget::heightChanged, this, &PositionerLayout::onParentSizeChanged);
        // parentWidget->installEventFilter(this);
    }
    markDirty();
}

void PositionerLayout::markDirty()
{
    if (!d->layouting) {
        d->layoutDirty = true;
        //        polish();
    } else {
        d->requeuePolish = true;
    }
}

void PositionerLayout::addItem(QWidget *item)
{
    //    connect(item, &QWidget::implicitWidthChanged, this, &PositionerLayout::markDirty);
    //    connect(item, &QWidget::implicitHeightChanged, this, &PositionerLayout::markDirty);
    //    connect(item, &QWidget::visibleChanged, this, &PositionerLayout::markDirty);
    //    connect(item, &QWidget::xChanged, this, &PositionerLayout::onItemPositionChanged);
    //    connect(item, &QWidget::yChanged, this, &PositionerLayout::onItemPositionChanged);
    item->installEventFilter(this);

    d->items.insert(item);

    debug("Add item to layout", item);

    markDirty();
}

void PositionerLayout::removeItem(QWidget *item)
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
    if (event->type() == PositionedItemChangedEvent::s_type) {
        markDirty();
        return false;
    }

    return QWidget::eventFilter(target, event);
}

void PositionerLayout::updatePolish()
{
    if (!d->layoutDirty) {
        return;
    }

    if (!parentWidget()->isVisible()) {
        return;
    }

    d->layouting = true;
    d->layoutDirty = false;

    Layout layout;

    debug("Performing layout for positioner of", parentWidget());

    for (const auto &item : std::as_const(d->items)) {
        auto source = PositionerSource::Source::Layout;

        if (!item->isVisible()) {
            debug("  Ignoring invisible item", item);
            continue;
        }

        auto properties = m_properties;
        auto layoutProperties = properties->layout();

        Union::Properties::AlignmentPropertyGroup *alignment;
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

        LayoutItem layoutItem{
            .implicitSize = QSizeF{static_cast<qreal>(item->rect().width()), static_cast<qreal>(item->rect().height())},
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
            qWarning() << "Alignment" << horizontalAlignment << "not supported for horizontal alignment, item" << item << "will use Start alignment";
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

    Q_EMIT layoutFinished();
}

void PositionerLayout::onParentSizeChanged()
{
    auto newSize = parentWidget()->rect().size();
    if (newSize == d->parentSize) {
        return;
    }

    d->parentSize = newSize;
    markDirty();
}

void PositionerLayout::onItemPositionChanged()
{
    if (d->ignorePositionChange) {
        return;
    }

    markDirty();
}

#include "moc_PositionerLayout.cpp"
