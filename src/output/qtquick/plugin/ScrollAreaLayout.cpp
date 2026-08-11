// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>

#include "ScrollAreaLayout.h"

#include <QPointer>
#include <QQmlProperty>

#include <properties/StylePropertyGroup.h>

#include "QuickStyle.h"

#include "qtquick_logging.h"

using namespace Union::Properties;
using namespace Union::Quick;

using namespace Qt::StringLiterals;

namespace
{
/**
 * How often the calculation may reverse its decision within a single layout.
 *
 * Contents that wrap change size along with the space available to them, so
 * both showing and not showing a scroll bar can be a valid outcome. Once this
 * is reached, settle on showing them, as that always reaches all contents.
 */
constexpr int MaximumReversals = 4;
}

class ScrollAreaLayout::Private
{
public:
    QPointer<QQuickItem> container;
    QPointer<QQuickItem> flickable;
    QPointer<QQuickItem> verticalScrollBar;
    QPointer<QQuickItem> horizontalScrollBar;

    bool verticalVisible : 1 = false;
    bool horizontalVisible : 1 = false;
    bool dirty : 1 = true;
    bool layouting : 1 = false;
    bool requeuePolish : 1 = false;

    qreal reservedWidth = 0.0;
    qreal reservedHeight = 0.0;

    int reversals = 0;

    QSizeF parentSize;

    inline static const Union::Properties::LayoutPropertyGroup EmptyLayoutGroup;
};

ScrollAreaLayout::ScrollAreaLayout(QQuickItem *parent)
    : QQuickItem(parent)
    , d(std::make_unique<Private>())
{
    polish();
}

ScrollAreaLayout::~ScrollAreaLayout() = default;

void ScrollAreaLayout::itemChange(ItemChange change, const ItemChangeData &data)
{
    // The scroll area is our parent, which is only set after we are created.
    if (change == ItemParentHasChanged) {
        if (d->container) {
            d->container->disconnect(this);
            d->container->removeEventFilter(this);
        }

        d->container = data.item;

        if (d->container) {
            connect(d->container, &QQuickItem::widthChanged, this, &ScrollAreaLayout::markDirty);
            connect(d->container, &QQuickItem::heightChanged, this, &ScrollAreaLayout::markDirty);
            d->container->installEventFilter(this);
        }

        markDirty();
    }

    QQuickItem::itemChange(change, data);
}

void ScrollAreaLayout::markDirty()
{
    if (d->layouting) {
        // Our own changes made the contents change size, so redo the
        // calculation with the new sizes once this one is done.
        d->requeuePolish = true;
        return;
    }

    d->dirty = true;
    polish();
}

QQuickItem *ScrollAreaLayout::flickable() const
{
    return d->flickable;
}

void ScrollAreaLayout::setFlickable(QQuickItem *newFlickable)
{
    if (newFlickable == d->flickable) {
        return;
    }

    disconnectItem(d->flickable);

    d->flickable = newFlickable;

    if (d->flickable) {
        auto contentWidth = QQmlProperty(d->flickable, u"contentWidth"_s);
        contentWidth.connectNotifySignal(this, SLOT(markDirty()));
        auto contentHeight = QQmlProperty(d->flickable, u"contentHeight"_s);
        contentHeight.connectNotifySignal(this, SLOT(markDirty()));
    }

    // Previous decisions say nothing about what these contents need.
    d->reversals = 0;

    markDirty();
    Q_EMIT flickableChanged();
}

QQuickItem *ScrollAreaLayout::verticalScrollBar() const
{
    return d->verticalScrollBar;
}

void ScrollAreaLayout::setVerticalScrollBar(QQuickItem *newScrollBar)
{
    if (newScrollBar == d->verticalScrollBar) {
        return;
    }

    disconnectItem(d->verticalScrollBar);
    d->verticalScrollBar = newScrollBar;
    connectScrollBar(d->verticalScrollBar);

    markDirty();
    Q_EMIT verticalScrollBarChanged();
}

QQuickItem *ScrollAreaLayout::horizontalScrollBar() const
{
    return d->horizontalScrollBar;
}

void ScrollAreaLayout::setHorizontalScrollBar(QQuickItem *newScrollBar)
{
    if (newScrollBar == d->horizontalScrollBar) {
        return;
    }

    disconnectItem(d->horizontalScrollBar);
    d->horizontalScrollBar = newScrollBar;
    connectScrollBar(d->horizontalScrollBar);

    markDirty();
    Q_EMIT horizontalScrollBarChanged();
}

bool ScrollAreaLayout::verticalVisible() const
{
    return d->verticalVisible;
}

bool ScrollAreaLayout::horizontalVisible() const
{
    return d->horizontalVisible;
}

qreal ScrollAreaLayout::reservedWidth() const
{
    return d->reservedWidth;
}

qreal ScrollAreaLayout::reservedHeight() const
{
    return d->reservedHeight;
}

bool ScrollAreaLayout::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QuickStyleUpdatedEvent::s_type) {
        markDirty();
        return false;
    }

    return QQuickItem::eventFilter(target, event);
}

void ScrollAreaLayout::connectScrollBar(QQuickItem *scrollBar)
{
    if (!scrollBar) {
        return;
    }

    connect(scrollBar, &QQuickItem::implicitWidthChanged, this, &ScrollAreaLayout::markDirty);
    connect(scrollBar, &QQuickItem::implicitHeightChanged, this, &ScrollAreaLayout::markDirty);

    auto policy = QQmlProperty(scrollBar, u"policy"_s);
    policy.connectNotifySignal(this, SLOT(markDirty()));
}

void ScrollAreaLayout::disconnectItem(QQuickItem *item)
{
    if (item) {
        item->disconnect(this);
    }
}

void ScrollAreaLayout::updatePolish()
{
    if (!d->dirty) {
        return;
    }

    auto container = d->container.get();
    if (!container) {
        return;
    }

    d->dirty = false;
    d->layouting = true;

    const auto style = qobject_cast<QuickStyle *>(qmlAttachedPropertiesObject<QuickStyle>(container, true));
    const auto query = style ? style->query() : nullptr;
    if (!query || !query->properties()) {
        // Not resolved yet, try again next frame.
        d->layouting = false;
        markDirty();
        return;
    }

    const auto properties = query->properties();
    const auto &layoutGroup = properties->layout() ? *(properties->layout()) : Private::EmptyLayoutGroup;
    const auto padding = layoutGroup.padding() ? layoutGroup.padding()->toMargins() : QMarginsF{};

    // Uses the padding of the style rather than that of the scroll area, as the
    // latter includes the space reserved for the scroll bars.
    const auto maximumWidth = container->width() - padding.left() - padding.right();
    const auto maximumHeight = container->height() - padding.top() - padding.bottom();

    const auto contentWidth = d->flickable ? d->flickable->property("contentWidth").toReal() : 0.0;
    const auto contentHeight = d->flickable ? d->flickable->property("contentHeight").toReal() : 0.0;

    const auto verticalSpace = d->verticalScrollBar ? d->verticalScrollBar->implicitWidth() : 0.0;
    const auto horizontalSpace = d->horizontalScrollBar ? d->horizontalScrollBar->implicitHeight() : 0.0;

    // Showing one scroll bar reduces the space available to the other, so guess
    // at the vertical one, decide the horizontal one, then reconsider.
    const auto maybeVertical = contentHeight > maximumHeight;
    auto needsHorizontal = contentWidth > maximumWidth - (maybeVertical ? verticalSpace : 0.0);
    auto needsVertical = contentHeight > maximumHeight - (needsHorizontal ? horizontalSpace : 0.0);

    const auto applyPolicy = [](QQuickItem *scrollBar, bool needed) {
        if (!scrollBar) {
            return false;
        }

        switch (scrollBar->property("policy").toInt()) {
        case Qt::ScrollBarAlwaysOff:
            return false;
        case Qt::ScrollBarAlwaysOn:
            return true;
        default:
            return needed;
        }
    };

    needsVertical = applyPolicy(d->verticalScrollBar, needsVertical);
    needsHorizontal = applyPolicy(d->horizontalScrollBar, needsHorizontal);

    if (needsVertical != d->verticalVisible || needsHorizontal != d->horizontalVisible) {
        if (d->reversals >= MaximumReversals) {
            qCDebug(UNION_QTQUICK) << "Scroll bar visibility for" << container << "does not converge, keeping current state";
            d->verticalVisible = d->verticalVisible || needsVertical;
            d->horizontalVisible = d->horizontalVisible || needsHorizontal;
        } else {
            d->reversals++;
            d->verticalVisible = needsVertical;
            d->horizontalVisible = needsHorizontal;
        }

        // This makes the scroll area update its padding, which may change the
        // content sizes, which requeues us through markDirty().
        Q_EMIT scrollBarVisibilityChanged();
    } else {
        d->reversals = 0;
    }

    const auto newReservedWidth = d->verticalVisible ? verticalSpace : 0.0;
    const auto newReservedHeight = d->horizontalVisible ? horizontalSpace : 0.0;
    if (newReservedWidth != d->reservedWidth || newReservedHeight != d->reservedHeight) {
        d->reservedWidth = newReservedWidth;
        d->reservedHeight = newReservedHeight;
        Q_EMIT reservedSizeChanged();
    }

    if (container->size() != d->parentSize) {
        // A new size means a new layout, so start counting reversals over.
        d->parentSize = container->size();
        d->reversals = 0;
    }

    d->layouting = false;

    if (d->requeuePolish) {
        d->requeuePolish = false;
        markDirty();
    }
}

#include "moc_ScrollAreaLayout.cpp"
