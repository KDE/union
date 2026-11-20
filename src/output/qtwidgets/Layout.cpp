// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "Layout.h"

#include "StyleGroupWrapper.h"
#include "elements/AbstractElement.h"
#include "elements/TextSubElement.h"

using namespace Union::Widgets;
using namespace Union::Properties;

Layout::LayoutMap Layout::layout(AbstractElement *element)
{
    LayoutMap map;
    QList<LayoutItem> items;
    // QStringList subElements = subElementList;

    if (!element || !element->properties()) {
        return map;
    }

    // if (!m_elementProperties || m_elementList.isEmpty()) {
    //     return map;
    // }
    // If subelement list is empty, just use default widget item.
    // This ensures any custom components get layouted too.
    // if (subElements.empty()) {
    //     qCDebug(UNION_QTWIDGETS) << "No subelements given, using Widget placeholder for" << subElements;
    //     subElements = {ElementString::Widget};
    // }

    // Setup the container rectangle
    // auto currentHierarchy = m_elementList;
    const auto elementProperties = element->properties();
    const QRectF availableSpace = backgroundRectangle(element->rect(), elementProperties).marginsRemoved(element->propertyWrapper().padding());
    m_spacing = element->propertyWrapper().spacing();

    // Prepare all subelements
    const auto subElements = element->childElements();
    for (const auto &subElement : subElements) {
        // NOTE: Currently text and icon are part of the main element, but eventually
        // will be moved as their own elements<
        // if (subElement != ElementString::Icon && subElement != ElementString::Text) {
        //     currentHierarchy.append(createElement(subElement));
        // }
        // const auto properties = queryProperties(currentHierarchy);
        const auto properties = subElement->properties();
        if (!properties) {
            continue;
        }

        const auto propertyWrapper = subElement->propertyWrapper();
        Union::Properties::Alignment horizontalAlignment;
        Union::Properties::Alignment verticalAlignment;
        int order = 0;
        QRectF elementRect = availableSpace;
        // NOTE: For now icon and text are their own things, so check them separately.
        // in future this should be unnecessary.
        // if (subElement == ElementString::Icon) {
        //     // Some styleoptions provide us an icon size, use it if its not null
        //     auto iconSize = iconSizeFromOption(opt);
        //     if (!iconSize.isValid()) {
        //         elementRect.setWidth(properties->safePropertyLookup(0.0, &StylePropertyGroup::icon, &IconPropertyGroup::width));
        //         elementRect.setHeight(properties->safePropertyLookup(0.0, &StylePropertyGroup::icon, &IconPropertyGroup::height));
        //     } else {
        //         elementRect.setWidth(iconSize.width());
        //         elementRect.setHeight(iconSize.height());
        //     }
        //     horizontalAlignment = properties->safePropertyLookup(Union::Properties::Alignment::Unspecified,
        //                                                      &StylePropertyGroup::icon,
        //                                                      &IconPropertyGroup::alignment,
        //                                                      &AlignmentPropertyGroup::horizontal);
        //     verticalAlignment = properties->safePropertyLookup(Union::Properties::Alignment::Unspecified,
        //                                                    &StylePropertyGroup::icon,
        //                                                    &IconPropertyGroup::alignment,
        //                                                    &AlignmentPropertyGroup::vertical);
        //     order = properties->safePropertyLookup(0, &StylePropertyGroup::icon, &IconPropertyGroup::alignment, &AlignmentPropertyGroup::order);
        // } else* if (subElement == ElementString::Text || subElement == ElementString::ShortcutText) {
        if (auto textElement = qobject_cast<TextSubElement *>(subElement)) {
            horizontalAlignment = safePropertyLookup(properties,
                                                     Alignment::Unspecified,
                                                     &StylePropertyGroup::text,
                                                     &TextPropertyGroup::alignment,
                                                     &AlignmentPropertyGroup::horizontal);
            verticalAlignment = safePropertyLookup(properties,
                                                   Alignment::Unspecified,
                                                   &StylePropertyGroup::text,
                                                   &TextPropertyGroup::alignment,
                                                   &AlignmentPropertyGroup::vertical);
            auto optionText = textElement->text();
            // if we are a menuitem and have a shortcut, we need to split the text with /t and place them according their alignments
            const int tabPosition(optionText.indexOf(QLatin1Char('\t')));
            if (tabPosition >= 0) {
                QString accelerator(optionText.mid(tabPosition + 1));
                // if (subElement == ElementString::ShortcutText) {
                //     optionText = optionText.mid(tabPosition + 1);
                // } else {
                optionText = optionText.left(tabPosition);
                // }
            }
            // When layouting, ensure we take mnemonics into account
            auto textFlags = propertyWrapper.textFlags(subElement->direction());
            textFlags |= Qt::TextShowMnemonic;

            auto fontMetrics = QFontMetrics{QFont{}};
            if (subElement->widget()) {
                fontMetrics = QFontMetrics(subElement->widget()->font());
            }
            if (properties->text() && properties->text()->font()) {
                fontMetrics = QFontMetrics(properties->text()->font()->toQFont());
            }
            elementRect = fontMetrics.boundingRect(availableSpace.toRect(), textFlags, optionText);
            order = properties->safePropertyLookup(0, &StylePropertyGroup::text, &TextPropertyGroup::alignment, &AlignmentPropertyGroup::order);
        } else {
            elementRect.setWidth(properties->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width));
            elementRect.setHeight(properties->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height));
            horizontalAlignment = properties->safePropertyLookup(Union::Properties::Alignment::Unspecified,
                                                                 &StylePropertyGroup::layout,
                                                                 &LayoutPropertyGroup::alignment,
                                                                 &AlignmentPropertyGroup::horizontal);
            verticalAlignment = properties->safePropertyLookup(Union::Properties::Alignment::Unspecified,
                                                               &StylePropertyGroup::layout,
                                                               &LayoutPropertyGroup::alignment,
                                                               &AlignmentPropertyGroup::vertical);
            order = properties->safePropertyLookup(0, &StylePropertyGroup::layout, &LayoutPropertyGroup::alignment, &AlignmentPropertyGroup::order);
        }
        LayoutItem item = LayoutItem();
        item.element = subElement;
        item.horizontalAlignment = horizontalAlignment;
        item.verticalAlignment = verticalAlignment;
        item.order = order;
        item.rect = elementRect;
        items.append(item);
    }

    // Actual layouting starts here
    // QtWidgets containment is always within Widget, since we can't draw outside of a widget due
    // widgets limitations.

    // Create buckets
    LayoutBucket startBucket = createBucket(items, BucketType::Start, availableSpace);
    LayoutBucket centerBucket = createBucket(items, BucketType::Center, availableSpace);
    LayoutBucket endBucket = createBucket(items, BucketType::End, availableSpace);
    LayoutBucket fillBucket = createBucket(items, BucketType::Fill, availableSpace);
    auto startOffset = startBucket.rect.width();
    auto endOffSet = endBucket.rect.width();

    startBucket.rect.moveLeft(availableSpace.left());
    mapBucketItems(startBucket, map);

    endBucket.rect.moveRight(availableSpace.right());
    mapBucketItems(endBucket, map);

    auto spacing = element->propertyWrapper().spacing();

    // Do not apply offsets if the items do not even exist.
    // Remove the spacing to avoid resizing the item too much:
    // the spacing is already accounted in mapBucketItems.
    centerBucket.rect.moveCenter(availableSpace.center());
    if (startBucket.rect.intersects(centerBucket.rect)) {
        if (startBucket.items.count() > 0) {
            centerBucket.rect.setLeft(centerBucket.rect.left() + startOffset + spacing);
        }
    }
    if (centerBucket.rect.intersects(endBucket.rect)) {
        centerBucket.rect.setRight(availableSpace.right());
        if (endBucket.items.count() > 0) {
            centerBucket.rect.setRight(centerBucket.rect.right() - endOffSet - spacing);
        }
    }
    mapBucketItems(centerBucket, map);

    fillBucket.rect.setLeft(startBucket.rect.right());
    fillBucket.rect.setRight(endBucket.rect.left());
    mapBucketItems(fillBucket, map);

    m_layoutMap = map;

    return map;
}

Layout::LayoutBucket Layout::createBucket(const QList<LayoutItem> &items, const BucketType &type, const QRectF &containerRect) const
{
    LayoutBucket bucket;
    bucket.type = type;
    bucket.rect = containerRect;

    for (auto &item : items) {
        switch (item.horizontalAlignment) {
        case Alignment::Unspecified:
        case Alignment::StackCenter:
        case Alignment::StackFill:
        case Alignment::Start:
            if (bucket.type == BucketType::Start) {
                bucket.items.append(item);
            }
            break;
        case Alignment::Center:
            if (bucket.type == BucketType::Center) {
                bucket.items.append(item);
            }
            break;
        case Alignment::End:
            if (bucket.type == BucketType::End) {
                bucket.items.append(item);
            }
            break;
        case Alignment::Fill:
            if (bucket.type == BucketType::Fill) {
                bucket.items.append(item);
            }
            break;
        }
    }

    // Sort the items according their order
    std::sort(bucket.items.begin(), bucket.items.end(), [bucket](const LayoutItem &lhs, const LayoutItem &rhs) {
        return lhs.order < rhs.order;
    });

    // No need for spacing as there are no items
    bucket.rect = bucket.items.count() > 0 ? resizeBucket(bucket) : QRectF();
    return bucket;
}

void Layout::mapBucketItems(LayoutBucket &bucket, LayoutMap &map) const
{
    auto bucketRect = bucket.rect;
    int space = bucket.items.count() > 0 ? m_spacing : 0;
    for (auto &item : bucket.items) {
        const auto itemWidth = item.rect.width() + space;
        const auto itemHeight = item.rect.height() + space;

        // For stackcenter/stackfill, we just want to center the rectangle based on its size.
        if (item.horizontalAlignment == Union::Properties::Alignment::Center
            && (item.verticalAlignment == Union::Properties::Alignment::StackCenter || item.verticalAlignment == Union::Properties::Alignment::StackFill)) {
            item.rect = centerRect(bucketRect.toRect(), item.rect.width(), item.rect.height());
        } else {
            switch (item.horizontalAlignment) {
            case Alignment::Unspecified:
            case Alignment::StackCenter:
            case Alignment::StackFill:
            case Alignment::Start:
            case Alignment::End:
                item.rect.moveLeft(bucketRect.left());
                bucketRect.setLeft(item.rect.left() + itemWidth);
                break;
            case Alignment::Center:
                if (bucket.items.count() > 1) {
                    item.rect.moveLeft(bucketRect.left());
                    bucketRect.setLeft(item.rect.left() + itemWidth);
                } else {
                    item.rect.moveCenter(bucketRect.center());
                }
                break;
            case Alignment::Fill:
                item.rect.setLeft(bucketRect.left());
                item.rect.setRight(bucketRect.right());
                bucketRect.setLeft(item.rect.left() + itemWidth);
                break;
            }
        }

        switch (item.verticalAlignment) {
        case Union::Properties::Alignment::Unspecified:
        case Union::Properties::Alignment::Start:
            item.rect.moveTop(bucketRect.top());
            break;
            // We can safely center the element within its rectangle here
        case Union::Properties::Alignment::Center:
            item.rect.moveCenter(QPoint(item.rect.center().x(), bucketRect.center().y()));
            break;
        case Union::Properties::Alignment::End:
            item.rect.moveBottom(bucketRect.bottom());
            break;
        case Union::Properties::Alignment::Fill:
            item.rect.setTop(bucketRect.top());
            item.rect.setBottom(bucketRect.bottom());
            break;
        case Union::Properties::Alignment::StackFill:
        case Union::Properties::Alignment::StackCenter:
            item.rect.moveTop(bucketRect.top());
            bucketRect.moveTop(item.rect.top() + itemHeight);
            break;
        }

        map[item.element] = item;
    }
}

QRectF Layout::resizeBucket(const LayoutBucket &bucket) const
{
    qreal width = 0;
    qreal height = 0;
    QRectF bucketRect = bucket.rect;
    bool stacked = false;
    auto space = bucket.items.count() > 0 ? m_spacing : 0;

    // Check if we are stacking or not
    for (auto &item : bucket.items) {
        if (item.verticalAlignment == Union::Properties::Alignment::StackFill || item.verticalAlignment == Union::Properties::Alignment::StackCenter) {
            stacked = true;
            break;
        }
    }

    for (auto &item : bucket.items) {
        const qreal itemWidth = item.rect.width() + space;
        const qreal itemHeight = item.rect.height();
        if (stacked) {
            width = std::max(bucketRect.width(), itemWidth);
            height += (itemHeight + space);
        } else {
            width += itemWidth;
            height = std::max(bucketRect.height(), itemHeight);
        }
    }
    bucketRect.setWidth(width);
    bucketRect.setHeight(height);

    return bucketRect;
}

// QRectF Layout::unifiedRect(QMap<QString, LayoutItem> layoutMap)
// {
// }
