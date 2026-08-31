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
    QMarginsF padding;
    if (paddingProperties->layout()) {
        auto width = paddingProperties->layout()->width().value_or(1);
        auto height = paddingProperties->layout()->height().value_or(1);
        preferredSize = QSize(width, height);
        if (paddingProperties->layout()->padding()) {
            padding = paddingProperties->layout()->padding()->toMargins().toMargins();
        }
        if (paddingProperties->layout()->inset()) {
            padding += paddingProperties->layout()->inset()->toMargins().toMargins();
        }
    }
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
        QRectF textRect = m_layoutMap[ElementString::Text].rect;
        int textFlags = Qt::AlignLeading | Qt::AlignVCenter;
        const bool enabled = m_styleOption->state.testFlag(QStyle::State_Enabled);
        QColor penColor = m_styleOption->palette.text().color();
        if (m_backgroundProperties->text()) {
            auto textColor = m_backgroundProperties->text()->color();
            if (textColor) {
                penColor = textColor->toQColor();
            }
            textFlags = textFlagsFromProperties(m_backgroundProperties, true);
        }
        painter->save();
        if (m_backgroundProperties->text() && m_backgroundProperties->text()->font().has_value()) {
            painter->setFont(m_backgroundProperties->text()->font().value());
        }
        painter->setPen(penColor);
        m_style->drawItemText(painter, textRect.toRect(), textFlags, m_styleOption->palette, enabled, m_text);
        painter->restore();
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

QMarginsF AbstractElement::padding() const
{
    if (m_isValid && m_backgroundProperties && m_backgroundProperties->layout() && m_backgroundProperties->layout()->padding()) {
        return m_backgroundProperties->layout()->padding()->toMargins();
    }
    return QMarginsF();
}

QMarginsF AbstractElement::borderSize() const
{
    if (m_isValid && m_backgroundProperties && m_backgroundProperties->border()) {
        return m_backgroundProperties->border()->sizes();
    }
    return QMarginsF();
}

qreal AbstractElement::height() const
{
    if (m_isValid && m_backgroundProperties && m_backgroundProperties->layout()) {
        return m_backgroundProperties->layout()->height().value_or(1);
    }
    return m_styleOption->rect.height();
}

qreal AbstractElement::width() const
{
    if (m_isValid && m_backgroundProperties && m_backgroundProperties->layout()) {
        return m_backgroundProperties->layout()->width().value_or(1);
    }
    return m_styleOption->rect.width();
}

qreal AbstractElement::spacing() const
{
    if (m_isValid && m_backgroundProperties && m_backgroundProperties->layout()) {
        return m_backgroundProperties->layout()->spacing().value_or(1);
    }
    return 0;
}

QSizeF AbstractElement::indicatorSize() const
{
    if (m_isValid && m_indicatorProperties && m_indicatorProperties->layout()) {
        auto width = m_indicatorProperties->layout()->width().value_or(0);
        auto height = m_indicatorProperties->layout()->height().value_or(0);
        return QSizeF(width, height);
    }
    return QSizeF();
}

QSizeF AbstractElement::iconSize() const
{
    if (m_isValid && m_backgroundProperties && m_backgroundProperties->icon()) {
        auto width = m_backgroundProperties->icon()->width().value_or(1);
        auto height = m_backgroundProperties->icon()->height().value_or(1);
        return QSizeF(width, height);
    }
    return QSizeF();
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

    // TODO: Go through all elements, create rectangles for them
    // Then place and resize those rectangles according to hierarchy
    // Use the original opt->rect as the main container
    // move any subelements in it according their given rules

    // Get spacing for main item
    auto properties = queryProperties(elements);
    QRectF availableSpace = backgroundRectangle(opt, properties);
    int globalSpacing = 0;
    QMarginsF padding;
    if (properties->layout()->padding()) {
        padding = properties->layout()->padding()->toMargins().toMargins();
    }
    availableSpace = availableSpace.marginsRemoved(padding);
    if (subElements.count() > 1) {
        globalSpacing = properties->layout()->spacing().value_or(0);
    }
    auto currentHierarchy = elements;
    // this could be turned into its own method
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
                elementRect.setWidth(properties->icon()->width().value_or(0));
                elementRect.setHeight(properties->icon()->height().value_or(0));
            }
            horizontalAlignment = properties->icon()->alignment()->horizontal().value_or(Union::Properties::Alignment::Unspecified);
            verticalAlignment = properties->icon()->alignment()->vertical().value_or(Union::Properties::Alignment::Unspecified);
            order = properties->icon()->alignment()->order().value_or(0);
        } else if (subElement == ElementString::Text || subElement == ElementString::ShortcutText) {
            horizontalAlignment = properties->text()->alignment()->horizontal().value_or(Union::Properties::Alignment::Unspecified);
            verticalAlignment = properties->text()->alignment()->vertical().value_or(Union::Properties::Alignment::Unspecified);
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
            auto textFlags = textFlagsFromProperties(properties, true);
            textFlags |= Qt::TextShowMnemonic;
            auto fontMetrics = opt->fontMetrics;
            if (properties->text() && properties->text()->font().has_value()) {
                fontMetrics = QFontMetrics(properties->text()->font().value());
            }
            elementRect = fontMetrics.boundingRect(availableSpace.toRect(), textFlags, optiontext);
            order = properties->text()->alignment()->order().value_or(0);
        } else {
            elementRect.setWidth(properties->layout()->width().value_or(0));
            elementRect.setHeight(properties->layout()->height().value_or(0));
            horizontalAlignment = properties->layout()->alignment()->horizontal().value_or(Union::Properties::Alignment::Unspecified);
            verticalAlignment = properties->layout()->alignment()->vertical().value_or(Union::Properties::Alignment::Unspecified);
            order = properties->layout()->alignment()->order().value_or(0);
        }
        LayoutItem item = LayoutItem();
        item.elementName = subElement;
        item.horizontalAlignment = horizontalAlignment;
        item.verticalAlignment = verticalAlignment;
        item.order = order;
        item.rect = elementRect;
        items.append(item);
    }

    // Sort the list according to order. Set any filled items as last
    std::sort(items.begin(), items.end(), [](const LayoutItem &lhs, const LayoutItem &rhs) {
        if (lhs.horizontalAlignment == rhs.horizontalAlignment || lhs.verticalAlignment == rhs.verticalAlignment) {
            // We reverse the order here to make sure the layouter reads this in correct order (0 1 2 instead of 2 1 0)
            if (lhs.horizontalAlignment == Union::Properties::Alignment::End || lhs.verticalAlignment == Union::Properties::Alignment::End) {
                return lhs.order > rhs.order;
            } else {
                return lhs.order < rhs.order;
            }
        }
        return false;
    });

    // Actual layouting starts here
    // QtWidgets containment is always within Widget, since we can't draw outside of a widget due
    // widgets limitations.

    int counter = 1;
    int spacing = globalSpacing;
    QRectF horizontalSpace = availableSpace;
    QRectF verticalSpace = availableSpace;

    // First, layout the start/end only
    for (auto &item : items) {
        // Skip spacing for last/only item
        if (counter >= items.count()) {
            spacing = 0;
        }

        auto itemWidth = item.rect.width() + spacing;
        auto itemHeight = item.rect.height() + spacing;
        switch (item.horizontalAlignment) {
        case Union::Properties::Alignment::StackFill:
        case Union::Properties::Alignment::StackCenter:
            qCWarning(UNION_QTWIDGETS) << "StackFill/StackCenter is not supported for horizontal alignment!";
        case Union::Properties::Alignment::Unspecified:
        case Union::Properties::Alignment::Start:
            item.rect.moveLeft(horizontalSpace.left());
            horizontalSpace.setLeft(item.rect.left() + itemWidth);
            break;
        case Union::Properties::Alignment::Center:
            // Center is bit confusing. It is meant to center the drawing inside the rectangle,
            // so we do that for stackcenter/stackfill items.
            if (item.horizontalAlignment == Union::Properties::Alignment::Center
                && (item.verticalAlignment == Union::Properties::Alignment::StackCenter || item.verticalAlignment == Union::Properties::Alignment::StackFill)) {
                item.rect = centerRect(horizontalSpace.toRect(), item.rect.width(), item.rect.height());
            } else {
                // When layouting normally we need to move it to next to the other item anyway.
                if (items.count() > 1) {
                    item.rect.moveLeft(horizontalSpace.left());
                    horizontalSpace.setLeft(item.rect.left() + itemWidth);
                } else {
                    // For single items, we can just center it completely
                    item.rect.moveCenter(availableSpace.center());
                }
            }
            break;
        case Union::Properties::Alignment::End:
            item.rect.moveRight(horizontalSpace.right());
            horizontalSpace.setRight(item.rect.right() - itemWidth);
            break;
        default:
            break;
        }

        switch (item.verticalAlignment) {
        case Union::Properties::Alignment::Unspecified:
        case Union::Properties::Alignment::Start:
            item.rect.moveTop(verticalSpace.top());
            verticalSpace.setTop(item.rect.top() + itemHeight);
            break;
            // We can safely center the element within its rectangle here
        case Union::Properties::Alignment::Center:
            item.rect.moveCenter(QPoint(item.rect.center().x(), verticalSpace.center().y()));
            break;
        case Union::Properties::Alignment::End:
            item.rect.moveBottom(verticalSpace.bottom());
            verticalSpace.setBottom(item.rect.bottom() - itemHeight);
            break;
        default:
            break;
        }

        map[item.elementName] = item;
        counter++;
    }

    // Then, layout the fills and stacks
    counter = 0;
    spacing = globalSpacing;
    for (auto &item : items) {
        // Skip spacing for last/only item
        if (counter >= items.count()) {
            spacing = 0;
        }

        auto itemHeight = item.rect.height() + spacing;
        switch (item.horizontalAlignment) {
        case Union::Properties::Alignment::Fill:
            item.rect.moveLeft(horizontalSpace.left());
            item.rect.setRight(horizontalSpace.right());
            break;
        default:
            break;
        }

        switch (item.verticalAlignment) {
        case Union::Properties::Alignment::Fill:
            item.rect.setTop(verticalSpace.top());
            item.rect.setBottom(verticalSpace.bottom());
            break;
        case Union::Properties::Alignment::StackFill:
        case Union::Properties::Alignment::StackCenter:
            item.rect.moveTop(verticalSpace.top());
            verticalSpace.moveTop(item.rect.top() + itemHeight);
            break;
        default:
            break;
        }

        map[item.elementName] = item;
        counter++;
    }

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
