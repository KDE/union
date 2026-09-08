// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "AbstractElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include "qtwidgets_logging.h"
#include <ElementQuery.h>
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>
#include <StyleRegistry.h>

using namespace Qt::StringLiterals;
using namespace Union::Properties;

AbstractElement::AbstractElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : QObject(nullptr)
    , m_styleOption(option)
    , m_style(style)
    , m_widget(widget)
{
}

AbstractElement::~AbstractElement()
{
}

QIcon AbstractElement::icon() const
{
    return m_icon;
}

void AbstractElement::setIcon(const QIcon &icon)
{
    m_icon = icon;
}

bool AbstractElement::hasIcon() const
{
    return !m_icon.isNull();
}

QString AbstractElement::text() const
{
    return m_text;
}

void AbstractElement::setText(const QString &text)
{
    m_text = text;
}

bool AbstractElement::hasText() const
{
    return !m_text.isEmpty();
}

QIcon AbstractElement::indicator() const
{
    return m_indicator;
}

void AbstractElement::setIndicator(const QIcon &indicator)
{
    m_indicator = indicator;
}

bool AbstractElement::hasIndicator() const
{
    return !m_indicator.isNull();
}

bool AbstractElement::isValid() const
{
    return m_isValid;
}

void AbstractElement::layout()
{
    // Background and content is separate
    m_backgroundElementList = prepareElements(m_styleOption, m_widget);
    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_layoutMap = layoutMap(m_backgroundElementList, m_styleOption, m_subElementList);
    }

    m_contentElementList = prepareElements(m_styleOption, m_widget, m_subElementList);
    if (!m_contentElementList.isEmpty()) {
        m_contentProperties = queryProperties(m_contentElementList);
        m_isValid = true;
    } else {
        m_isValid = false;
        qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
    }
}

QSizeF AbstractElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    return applyPaddingToSize(contentsSizeFromStyle);
}

QRectF AbstractElement::subElementRect(QStyle::SubElement element) const
{
    Q_UNUSED(element);
    return QRect();
}

QRectF AbstractElement::subControlRect(QStyle::SubControl subControl) const
{
    Q_UNUSED(subControl);
    return QRect();
}

qreal AbstractElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    Q_UNUSED(pixelMetric);
    return 0;
}

void AbstractElement::updateSubElementList()
{
}

void AbstractElement::update()
{
}

void AbstractElement::draw(QPainter *painter, DrawEnums enums) const
{
    Q_UNUSED(painter);
    qCWarning(UNION_QTWIDGETS) << "Drawing not implemented for " << enums.ComplexControl << enums.ControlElement << enums.PrimitiveElement;
}

QVariantMap AbstractElement::elementAttributes() const
{
    return QVariantMap();
}

QStringList AbstractElement::elementHints() const
{
    return QStringList();
}

Union::Element::States AbstractElement::elementStates() const
{
    Union::Element::States states;
    if (!m_styleOption) {
        return states;
    }
    if (m_styleOption->state.testFlag(QStyle::State_None)) {
        return states;
    }

    states.setFlag(Union::Element::State::Hovered, m_styleOption->state.testFlag(QStyle::State_MouseOver));
    states.setFlag(Union::Element::State::ActiveFocus, m_styleOption->state.testFlag(QStyle::State_HasFocus));
    states.setFlag(Union::Element::State::VisualFocus,
                   m_styleOption->state.testFlag(QStyle::State_KeyboardFocusChange) && m_styleOption->state.testFlag(QStyle::State_HasFocus));
    states.setFlag(Union::Element::State::Disabled, !m_styleOption->state.testFlag(QStyle::State_Enabled));
    states.setFlag(Union::Element::State::Highlighted, m_styleOption->state.testFlag(QStyle::State_Selected));

    if (m_styleOption->state.testFlag(QStyle::State_On)) {
        states.setFlag(Union::Element::State::Checked);
    }
    if (m_styleOption->state.testFlag(QStyle::State_Off)) {
        states.setFlag(Union::Element::State::Checked, false);
    }

    if (m_styleOption->state.testFlag(QStyle::State_Sunken)) {
        states.setFlag(Union::Element::State::Pressed);
    }
    if (m_styleOption->state.testFlag(QStyle::State_Raised)) {
        states.setFlag(Union::Element::State::Pressed, false);
    }

    return states;
}

QSizeF AbstractElement::applyPaddingToSize(QSizeF oldSize, PaddingDirection direction, Union::Properties::StylePropertyGroup *properties) const
{
    if (!m_isValid) {
        return oldSize;
    }
    auto paddingProperties = properties;
    // Use background properties as the default
    if (!properties) {
        paddingProperties = m_backgroundProperties;
    }
    QSizeF preferredSize = oldSize;
    QSizeF size = preferredSize;
    QMarginsF padding =
        safePropertyLookup(paddingProperties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
    QMarginsF inset =
        safePropertyLookup(paddingProperties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::inset, &SizePropertyGroup::toMargins);
    auto width = safePropertyLookup(paddingProperties, 0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width);
    auto height = safePropertyLookup(paddingProperties, 0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height);
    preferredSize = QSizeF(width, height);
    padding += inset;
    if (direction == PaddingDirection::Inward) {
        size = size.shrunkBy(padding);
        if (size.width() < 0) {
            size.setWidth(0);
        }
        if (size.height() < 0) {
            size.setHeight(0);
        }
    } else {
        size = size.grownBy(padding);
        if (size.width() < preferredSize.width()) {
            size.setWidth(preferredSize.width());
        }
        if (size.height() < preferredSize.height()) {
            size.setHeight(preferredSize.height());
        }
    }
    return size.toSize();
}

void AbstractElement::drawBackground(QPainter *painter) const
{
    if (m_isValid && m_styleOption) {
        drawBackgroundRectangle(painter, m_styleOption->rect, m_backgroundProperties);
    }
}

void AbstractElement::drawFrame(QPainter *painter) const
{
    if (m_isValid && m_styleOption) {
        drawBackgroundRectangle(painter, m_styleOption->rect, m_backgroundProperties, BackgroundParts::FrameOnly);
    }
}

void AbstractElement::drawPanel(QPainter *painter) const
{
    if (m_isValid && m_styleOption) {
        drawBackgroundRectangle(painter, m_styleOption->rect, m_backgroundProperties, BackgroundParts::PanelOnly);
    }
}

void AbstractElement::drawText(QPainter *painter) const
{
    if (hasText() && m_isValid) {
        drawTextAtRect(painter, m_text, m_layoutMap[ElementString::Text].rect, m_backgroundProperties);
    }
}

void AbstractElement::drawIcon(QPainter *painter) const
{
    if (hasIcon() && m_isValid) {
        drawIconAtRect(painter, m_icon, m_layoutMap[ElementString::Icon].rect);
    }
}

void AbstractElement::drawIndicator(QPainter *painter) const
{
    if (hasIndicator() && m_isValid) {
        drawIconAtRect(painter, m_indicator, m_layoutMap[ElementString::Indicator].rect);
    }
}

void AbstractElement::drawIconAtRect(QPainter *painter, const QIcon &icon, const QRectF rect) const
{
    if (m_isValid) {
        bool enabled = true;
        if (m_styleOption) {
            enabled = m_styleOption->state.testFlag(QStyle::State_Enabled);
        }
        const qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : qApp->devicePixelRatio();
        auto iconSize = rect.size();
        const QPixmap pixmap = icon.pixmap(iconSize.toSize(), dpr, enabled ? QIcon::Normal : QIcon::Disabled);
        painter->save();
        m_style->drawItemPixmap(painter, rect.toRect(), Qt::AlignCenter, pixmap);
        painter->restore();
    }
}

void AbstractElement::drawTextAtRect(QPainter *painter, const QString &text, const QRectF &rect, Union::Properties::StylePropertyGroup *properties) const
{
    if (m_isValid && m_styleOption) {
        int textFlags = Qt::AlignLeading | Qt::AlignVCenter;
        const bool enabled = m_styleOption->state.testFlag(QStyle::State_Enabled);
        auto textColor = m_styleOption->palette.text().color();
        auto unionColor = safePropertyLookup(properties, Union::Color{}, &StylePropertyGroup::text, &TextPropertyGroup::color);
        auto font = safePropertyLookup(properties, painter->font(), &StylePropertyGroup::text, &TextPropertyGroup::font);
        if (unionColor.isValid()) {
            textColor = unionColor.toQColor();
        }
        textFlags = textFlagsFromProperties(properties);
        painter->save();
        painter->setPen(textColor);
        painter->setFont(font);
        m_style->drawItemText(painter, rect.toRect(), textFlags, m_styleOption->palette, enabled, elidedText(text, rect, properties));
        painter->restore();
    }
}

QMarginsF AbstractElement::padding() const
{
    return safePropertyLookup(m_backgroundProperties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
}

QMarginsF AbstractElement::borderSize() const
{
    return safePropertyLookup(m_backgroundProperties, QMarginsF{}, &StylePropertyGroup::border, &BorderPropertyGroup::sizes);
}

qreal AbstractElement::height() const
{
    if (m_backgroundProperties) {
        return m_backgroundProperties->safePropertyLookup(1.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height);
    }
    return m_styleOption->rect.height();
}

qreal AbstractElement::width() const
{
    if (m_backgroundProperties) {
        return m_backgroundProperties->safePropertyLookup(1.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width);
    }
    return m_styleOption->rect.height();
}

qreal AbstractElement::spacing() const
{
    return safePropertyLookup(m_backgroundProperties, 1.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::spacing);
}

QSizeF AbstractElement::indicatorSize() const
{
    auto width = safePropertyLookup(m_indicatorProperties, 0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width);
    auto height = safePropertyLookup(m_indicatorProperties, 0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height);
    return QSizeF(width, height);
}

QSizeF AbstractElement::iconSize() const
{
    auto width = safePropertyLookup(m_backgroundProperties, 0.0, &StylePropertyGroup::icon, &IconPropertyGroup::width);
    auto height = safePropertyLookup(m_backgroundProperties, 0.0, &StylePropertyGroup::icon, &IconPropertyGroup::height);
    return QSizeF(width, height);
}

qreal AbstractElement::averagePadding() const
{
    auto margins = padding();
    if (margins.isNull()) {
        return 0;
    }
    return (margins.left() + margins.right() + margins.top() + margins.bottom()) / 4;
}

qreal AbstractElement::averageHPadding() const
{
    auto margins = padding();
    if (margins.isNull()) {
        return 0;
    }
    return (margins.left() + margins.right()) / 2;
}

qreal AbstractElement::averageVPadding() const
{
    auto margins = padding();
    if (margins.isNull()) {
        return 0;
    }
    return (margins.top() + margins.bottom()) / 2;
}

qreal AbstractElement::averageBorderSize() const
{
    auto margins = borderSize();
    if (margins.isNull()) {
        return 0;
    }
    return (margins.left() + margins.right() + margins.top() + margins.bottom()) / 4;
}

QString AbstractElement::elidedText(const QString &text, const QRectF &textRect, Union::Properties::StylePropertyGroup *properties) const
{
    if (!text.isEmpty() && m_styleOption) {
        auto elide = safePropertyLookup(properties, TextElide::None, &StylePropertyGroup::text, &TextPropertyGroup::elide);
        const qreal elideWidth = textRect.width() < m_styleOption->rect.width() ? textRect.width() : m_styleOption->rect.width();
        return m_styleOption->fontMetrics.elidedText(text, toQtElideMode(elide), elideWidth);
    }
    return text;
}

QSizeF AbstractElement::querySize(QStringList targetHierarchy) const
{
    auto elements = prepareElements(m_styleOption, m_widget, targetHierarchy);
    if (elements.isEmpty()) {
        return QSize(0, 0);
    }
    auto properties = queryProperties(elements);
    if (properties && properties->layout()) {
        return QSize(properties->layout()->width().value_or(0), properties->layout()->height().value_or(0));
    }
    return QSize(0, 0);
}

Union::ElementList AbstractElement::prepareElements(const QStyleOption *opt, const QWidget *widget, QStringList targetHierarchy) const
{
    Union::ElementList elements;
    QStringList elementTypes = {};

    if (widget) {
        elementTypes = widget->property(property_union_member_list).toStringList();
        if (elementTypes.isEmpty()) {
            elementTypes = widgetToElementHierarchy(widget);
        }
    } else {
        elementTypes = {styleOptionToElementName(opt)};
    }

    // Meld duplicate elements that appear next to each other
    if (elementTypes.isEmpty()) {
        elementTypes.append(targetHierarchy);
    } else {
        for (const auto &target : targetHierarchy) {
            if (elementTypes.last() != target) {
                elementTypes.append(target);
            }
        }
    }

    for (const auto &elementType : elementTypes) {
        elements.append(createElement(elementType));
    }
    return elements;
}

Union::Properties::StylePropertyGroup *AbstractElement::queryProperties(const Union::ElementList &elements) const
{
    Q_ASSERT(!elements.isEmpty());
    const auto style = Union::StyleRegistry::instance()->defaultStyle();
    const auto query = std::make_unique<Union::ElementQuery>(style);
    query->setElements(elements);
    query->execute();
    return query->properties();
}

QMap<QString, LayoutItem> AbstractElement::layoutMap(const Union::ElementList &elements, const QStyleOption *opt, const QStringList &subElementList) const
{
    QMap<QString, LayoutItem> map;
    QList<LayoutItem> items;
    QStringList subElements = subElementList;

    // If subelement list is empty, just use default widget item.
    // This ensures any custom components get layouted too.
    if (subElements.empty()) {
        qCDebug(UNION_QTWIDGETS) << "No sublements given, using Widget placeholder for" << elements << opt->type << opt->styleObject;
        subElements = {ElementString::Widget};
    }

    // Setup the container rectangle
    auto properties = queryProperties(elements);
    QRectF availableSpace = backgroundRectangle(opt, properties);
    QMarginsF padding = safePropertyLookup(properties, QMarginsF(), &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
    availableSpace = availableSpace.marginsRemoved(padding);
    auto currentHierarchy = elements;

    // Prepare all subelements
    for (const auto &subElement : subElements) {
        // NOTE: Currently text and icon are part of the main element, but eventually
        // will be moved as their own elements
        if (subElement != ElementString::Icon && subElement != ElementString::Text) {
            currentHierarchy.append(createElement(subElement));
        }
        properties = queryProperties(currentHierarchy);
        Union::Properties::Alignment horizontalAlignment;
        Union::Properties::Alignment verticalAlignment;
        int order = 0;
        QRectF elementRect = availableSpace;
        // NOTE: For now icon and text are their own things, so check them separately.
        // in future this should be unnecessary.
        if (subElement == ElementString::Icon) {
            // Toolbutton can override the icon size
            if (const auto *toolButtonOption = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
                elementRect.setWidth(toolButtonOption->iconSize.width());
                elementRect.setHeight(toolButtonOption->iconSize.height());
            } else {
                elementRect.setWidth(properties->safePropertyLookup(0.0, &StylePropertyGroup::icon, &IconPropertyGroup::width));
                elementRect.setHeight(properties->safePropertyLookup(0.0, &StylePropertyGroup::icon, &IconPropertyGroup::height));
            }
            horizontalAlignment = properties->safePropertyLookup(Union::Properties::Alignment::Unspecified,
                                                                 &StylePropertyGroup::icon,
                                                                 &IconPropertyGroup::alignment,
                                                                 &AlignmentPropertyGroup::horizontal);
            verticalAlignment = properties->safePropertyLookup(Union::Properties::Alignment::Unspecified,
                                                               &StylePropertyGroup::icon,
                                                               &IconPropertyGroup::alignment,
                                                               &AlignmentPropertyGroup::vertical);
            order = properties->safePropertyLookup(0, &StylePropertyGroup::icon, &IconPropertyGroup::alignment, &AlignmentPropertyGroup::order);
        } else if (subElement == ElementString::Text || subElement == ElementString::ShortcutText) {
            horizontalAlignment = properties->safePropertyLookup(Union::Properties::Alignment::Unspecified,
                                                                 &StylePropertyGroup::text,
                                                                 &TextPropertyGroup::alignment,
                                                                 &AlignmentPropertyGroup::horizontal);
            verticalAlignment = properties->safePropertyLookup(Union::Properties::Alignment::Unspecified,
                                                               &StylePropertyGroup::text,
                                                               &TextPropertyGroup::alignment,
                                                               &AlignmentPropertyGroup::vertical);
            auto optiontext = textFromOption(opt);
            // if we are a menuitem and have a shortcut, we need to split the text with /t and place them according their alignments
            const int tabPosition(optiontext.indexOf(QLatin1Char('\t')));
            if (tabPosition >= 0) {
                QString accelerator(optiontext.mid(tabPosition + 1));
                if (subElement == ElementString::ShortcutText) {
                    optiontext = optiontext.mid(tabPosition + 1);
                } else {
                    optiontext = optiontext.left(tabPosition);
                }
            }
            // When layouting, ensure we take mnemonics into account
            auto textFlags = textFlagsFromProperties(properties);
            textFlags |= Qt::TextShowMnemonic;
            auto fontMetrics = opt->fontMetrics;
            auto styleFont = properties->safePropertyLookup(std::optional<QFont>(), &StylePropertyGroup::text, &TextPropertyGroup::font);
            if (styleFont.has_value()) {
                fontMetrics = QFontMetrics(styleFont.value());
            }
            elementRect = fontMetrics.boundingRect(availableSpace.toRect(), textFlags, optiontext);
            order = properties->text()->alignment()->order().value_or(0);
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
        item.elementName = subElement;
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

    // Do not apply offsets if the items do not even exist.
    // Remove the spacing to avoid resizing the item too much:
    // the spacing is already accounted in mapBucketItems.
    centerBucket.rect.moveCenter(availableSpace.center());
    centerBucket.rect.setLeft(availableSpace.left() - startBucket.spacing);
    if (startBucket.items.count() > 0) {
        centerBucket.rect.setLeft(centerBucket.rect.left() + startOffset);
    }
    centerBucket.rect.setRight(availableSpace.right() + endBucket.spacing);
    if (endBucket.items.count() > 0) {
        centerBucket.rect.setRight(centerBucket.rect.right() - endOffSet);
    }
    mapBucketItems(centerBucket, map);

    fillBucket.rect.setLeft(startBucket.rect.right());
    fillBucket.rect.setRight(endBucket.rect.left());
    mapBucketItems(fillBucket, map);

    return map;
}

Union::Element::Ptr AbstractElement::createElement(const QString &name) const
{
    auto unionElement = Union::Element::create();
    unionElement->setType(name);
    unionElement->setStates(elementStates());
    unionElement->setHints(elementHints());
    unionElement->setAttributes(elementAttributes());
    return unionElement;
}

LayoutBucket AbstractElement::createBucket(const QList<LayoutItem> &items, const BucketType &type, const QRectF &containerRect) const
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
        // We reverse the order here to make sure the layouter reads this in correct order (0 1 2 instead of 2 1 0)
        if (bucket.type == BucketType::End) {
            return lhs.order > rhs.order;
        } else {
            return lhs.order < rhs.order;
        }
    });

    // No need for spacing as there are no items
    bucket.spacing = bucket.items.count() > 0 ? spacing() : 0;
    bucket.rect = bucket.items.count() > 0 ? resizeBucket(bucket) : QRectF();
    return bucket;
}

QRectF AbstractElement::resizeBucket(const LayoutBucket &bucket) const
{
    qreal width = 0;
    qreal height = 0;
    QRectF bucketRect = bucket.rect;
    for (auto &item : bucket.items) {
        const qreal itemWidth = item.rect.width() + bucket.spacing;
        const qreal itemHeight = item.rect.height();
        const bool stacked =
            (item.verticalAlignment == Union::Properties::Alignment::StackFill || item.verticalAlignment == Union::Properties::Alignment::StackCenter);

        if (stacked) {
            width = std::max(bucketRect.width(), itemWidth);
            height += (itemHeight + bucket.spacing);
        } else {
            width += itemWidth;
            height = std::max(bucketRect.height(), itemHeight);
        }
    }
    bucketRect.setWidth(width);
    bucketRect.setHeight(height);

    return bucketRect;
}

void AbstractElement::mapBucketItems(LayoutBucket &bucket, QMap<QString, LayoutItem> &map) const
{
    auto bucketRect = bucket.rect;
    int spacing = bucket.spacing;
    for (auto &item : bucket.items) {
        const auto itemWidth = item.rect.width() + spacing;
        const auto itemHeight = item.rect.height() + spacing;

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

        map[item.elementName] = item;
    }
    bucket.rect = bucketRect;
}
