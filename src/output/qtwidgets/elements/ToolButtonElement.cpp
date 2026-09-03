// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ToolButtonElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

ToolButtonElement::ToolButtonElement(const QStyleOptionToolButton *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_toolButtonOption(option)
    , m_hasIndicator(false)
    , m_hasArrows(false)
    , m_hasIcon(false)
    , m_hasText(false)
{
    update();
}

ToolButtonElement::~ToolButtonElement()
{
}

void ToolButtonElement::update()
{
    m_hasIndicator =
        m_toolButtonOption->features.testFlag(QStyleOptionToolButton::HasMenu) || m_toolButtonOption->features.testFlag(QStyleOptionToolButton::Menu);
    m_hasArrows = m_toolButtonOption->features.testFlag(QStyleOptionToolButton::Arrow) && m_toolButtonOption->toolButtonStyle != Qt::ToolButtonTextOnly;
    m_hasIcon = !m_toolButtonOption->icon.isNull() && m_toolButtonOption->toolButtonStyle != Qt::ToolButtonTextOnly;
    m_hasText = !m_toolButtonOption->text.isEmpty() && m_toolButtonOption->toolButtonStyle != Qt::ToolButtonIconOnly;
    setIcon(m_toolButtonOption->icon);
    setText(m_toolButtonOption->text);
    updateSubElementList();
    layout();
}

void ToolButtonElement::layout()
{
    m_indicatorElementList = prepareElements(m_toolButtonOption, m_widget, {ElementString::Indicator});
    setIndicator(QIcon());
    if (!m_indicatorElementList.isEmpty()) {
        m_indicatorProperties = queryProperties(m_indicatorElementList);
        setIndicator(m_style->unionIcon(m_indicatorProperties, QString()));
    }

    m_backgroundElementList = prepareElements(m_styleOption, m_widget);
    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_indicatorMap = layoutMap(m_backgroundElementList, m_styleOption, {ElementString::Indicator});
        layoutButtons();
        // Update layoutmap so that the text and icon are within the main button
        auto subopt = *m_toolButtonOption;
        subopt.rect = m_mainButtonRect.toRect();
        m_layoutMap = layoutMap(m_backgroundElementList, &subopt, m_subElementList);
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

void ToolButtonElement::updateSubElementList()
{
    m_subElementList.clear();
    if (m_hasIcon || m_hasArrows) {
        m_subElementList.append(ElementString::Icon);
    }
    if (m_hasText) {
        m_subElementList.append(ElementString::Text);
    }
}

QSizeF ToolButtonElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    QSizeF size = applyPaddingToSize(contentsSizeFromStyle);
    size = size.expandedTo(m_menuButtonRect.size());
    return size;
}

QRectF ToolButtonElement::subControlRect(QStyle::SubControl subControl) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "subControlRect for " << subControl << "is not valid";
        return QRect();
    }

    if (subControl == QStyle::SC_ToolButton) {
        return m_mainButtonRect;
    }
    if (subControl == QStyle::SC_ToolButtonMenu) {
        return m_menuButtonRect;
    }
    return QRect();
}

void ToolButtonElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_ToolButtonLabel:
        drawText(painter);
        break;
    }

    switch (enums.ComplexControl) {
    case QStyle::CC_ToolButton:
        drawBackground(painter);
        drawIcon(painter);
        drawText(painter);
        drawIndicator(painter);
        break;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_FrameButtonTool:
        drawFrame(painter);
        break;
    case QStyle::PE_PanelButtonTool:
        drawBackground(painter);
        break;
    }
}

void ToolButtonElement::drawIcon(QPainter *painter) const
{
    if (m_toolButtonOption->toolButtonStyle == Qt::ToolButtonTextOnly) {
        return;
    }

    QRectF iconRect = m_layoutMap[ElementString::Icon].rect;
    if (m_toolButtonOption->features.testFlag(QStyleOptionToolButton::Arrow)) {
        auto subopt = *m_toolButtonOption;
        subopt.rect = iconRect.toRect();
        switch (m_toolButtonOption->arrowType) {
        case Qt::LeftArrow:
            m_style->drawPrimitive(QStyle::PE_IndicatorArrowLeft, &subopt, painter);
            break;
        case Qt::RightArrow:
            m_style->drawPrimitive(QStyle::PE_IndicatorArrowRight, &subopt, painter);
            break;
        case Qt::UpArrow:
            m_style->drawPrimitive(QStyle::PE_IndicatorArrowUp, &subopt, painter);
            break;
        case Qt::DownArrow:
            m_style->drawPrimitive(QStyle::PE_IndicatorArrowDown, &subopt, painter);
            break;
        default:
            break;
        }
        return;
    } else if (hasIcon()) {
        drawIconAtRect(painter, m_icon, iconRect);
    }
}

void ToolButtonElement::drawIndicator(QPainter *painter) const
{
    auto rect = subControlRect(QStyle::SC_ToolButtonMenu);
    auto indicatorRect = m_indicatorMap[ElementString::Indicator].rect;
    indicatorRect.moveCenter(rect.center());
    drawBackgroundRectangle(painter, rect, m_indicatorProperties);
    drawIconAtRect(painter, m_indicator, indicatorRect);
}

QVariantMap ToolButtonElement::elementAttributes() const
{
    QVariantMap map;
    switch (m_toolButtonOption->toolButtonStyle) {
    case Qt::ToolButtonIconOnly:
        map[u"display"_s] = QVariant(u"icon-only"_s);
        break;
    case Qt::ToolButtonTextOnly:
        map[u"display"_s] = QVariant(u"text-only"_s);
        break;
    case Qt::ToolButtonTextBesideIcon:
        map[u"display"_s] = QVariant(u"text-beside-icon"_s);
        break;
    case Qt::ToolButtonTextUnderIcon:
        map[u"display"_s] = QVariant(u"text-under-icon"_s);
        break;
    default:
        return map;
    }
    return map;
}

QStringList ToolButtonElement::elementHints() const
{
    QStringList hints;
    if (m_toolButtonOption->features.testFlag(QStyleOptionToolButton::ToolButtonFeature::None)) {
        return hints;
    }

    auto style = arrowStyle();
    switch (style) {
    case ArrowStyle::None:
        break;
    case ArrowStyle::Menu:
        hints.append(u"with-menu"_s);
        break;
    case ArrowStyle::InlineArrow:
        hints.append(u"with-inline-arrow"_s);
        break;
    case ArrowStyle::MenuButton:
        hints.append(u"with-menu-button"_s);
        break;
    }
    if (!m_toolButtonOption->state.testFlag(QStyle::State_AutoRaise)) {
        hints.append(u"raised"_s);
    }
    // If menuButton is pressed, both sunken and raised are active
    if (m_toolButtonOption->state.testFlags({QStyle::State_Sunken, QStyle::State_Raised})) {
        hints.append(u"menu-open"_s);
    }

    return hints;
}

void ToolButtonElement::layoutButtons()
{
    m_mainButtonRect = m_toolButtonOption->rect;
    if (!m_hasIndicator) {
        return;
    }
    m_menuButtonRect = m_indicatorMap[ElementString::Indicator].rect;

    // With inline arrow, the main element does not care about making room to it,
    // as this arrow lives within the padding area.
    bool arrowInline = (arrowStyle() == ArrowStyle::InlineArrow);
    // Align the second button around the main button
    if (m_indicatorProperties && m_indicatorProperties->layout() && m_indicatorProperties->layout()->alignment()) {
        auto alignH = m_indicatorProperties->layout()->alignment()->horizontal().value_or(Union::Properties::Alignment::Unspecified);
        auto alignV = m_indicatorProperties->layout()->alignment()->vertical().value_or(Union::Properties::Alignment::Unspecified);

        QMarginsF padding;
        if (m_indicatorProperties->layout()->padding()) {
            padding = m_indicatorProperties->layout()->padding()->toMargins();
        }
        m_menuButtonRect = m_menuButtonRect.marginsAdded(padding);

        switch (alignH) {
        case Union::Properties::Alignment::Start:
            m_menuButtonRect.moveLeft(m_mainButtonRect.left());
            if (!arrowInline) {
                m_mainButtonRect.setLeft(m_menuButtonRect.right());
            }
            break;
        case Union::Properties::Alignment::Center:
            m_menuButtonRect.moveCenter(QPoint(m_mainButtonRect.center().x(), m_menuButtonRect.center().y()));
            break;
        case Union::Properties::Alignment::End:
        case Union::Properties::Alignment::Unspecified:
        case Union::Properties::Alignment::StackCenter:
            m_menuButtonRect.moveRight(m_mainButtonRect.right());
            if (!arrowInline) {
                m_mainButtonRect.setRight(m_menuButtonRect.left());
            }
            break;
        case Union::Properties::Alignment::StackFill:
        case Union::Properties::Alignment::Fill:
            m_menuButtonRect.setRight(m_mainButtonRect.right());
            m_menuButtonRect.setLeft(m_mainButtonRect.left());
            break;
        }

        switch (alignV) {
        case Union::Properties::Alignment::Start:
            m_menuButtonRect.moveTop(m_mainButtonRect.top());
            if (!arrowInline) {
                m_mainButtonRect.setTop(m_menuButtonRect.bottom());
            }
            break;
        case Union::Properties::Alignment::Center:
        case Union::Properties::Alignment::Unspecified:
        case Union::Properties::Alignment::StackCenter:
            m_menuButtonRect.moveCenter(QPoint(m_menuButtonRect.center().x(), m_mainButtonRect.center().y()));
            break;
        case Union::Properties::Alignment::End:
            m_menuButtonRect.moveBottom(m_mainButtonRect.bottom());
            if (!arrowInline) {
                m_mainButtonRect.setBottom(m_menuButtonRect.top());
            }
            break;
        case Union::Properties::Alignment::Fill:
        case Union::Properties::Alignment::StackFill:
            m_menuButtonRect.setTop(m_mainButtonRect.top());
            m_menuButtonRect.setBottom(m_mainButtonRect.bottom());
            break;
        }
    }
}

ArrowStyle ToolButtonElement::arrowStyle() const
{
    // Behavior taken from breeze
    const bool hasPopupMenu = (m_hasIndicator && m_toolButtonOption->features.testFlag(QStyleOptionToolButton::MenuButtonPopup));
    const bool hasInlineIndicator = (m_hasIndicator && !hasPopupMenu);
    const bool hasDelayedMenu = (hasInlineIndicator && m_toolButtonOption->features.testFlag(QStyleOptionToolButton::PopupDelay));
    const bool hasIcon = !m_toolButtonOption->icon.isNull() || (m_toolButtonOption->features.testFlag(QStyleOptionToolButton::Arrow));
    const bool iconOnly = m_toolButtonOption->toolButtonStyle == Qt::ToolButtonIconOnly || (m_toolButtonOption->text.isEmpty() && hasIcon);

    if (hasPopupMenu) {
        return ArrowStyle::MenuButton;
    }

    if (hasDelayedMenu) {
        return ArrowStyle::InlineArrow;
    }

    if (hasInlineIndicator && !iconOnly) {
        return ArrowStyle::Menu;
    }

    return ArrowStyle::None;
}
