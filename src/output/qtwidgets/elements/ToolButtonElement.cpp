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
    m_indicatorElementList = prepareElements(m_toolButtonOption, m_widget, {ElementString::Indicator});
    setIndicator(QIcon());
    if (!m_indicatorElementList.isEmpty()) {
        m_indicatorProperties = queryProperties(m_indicatorElementList);
        setIndicator(m_style->unionIcon(m_indicatorProperties, QString()));
    }
    setIcon(m_toolButtonOption->icon);
    setText(m_toolButtonOption->text);
    updateSubElementList();
    layout();
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
    if (m_hasIndicator) {
        m_subElementList.append(ElementString::Indicator);
    }
}

QSizeF ToolButtonElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    QSizeF size = subControlRect(QStyle::SC_ToolButton).size().boundedTo(contentsSizeFromStyle);
    size = applyPaddingToSize(size);

    if (m_indicatorProperties && m_indicatorProperties->layout()) {
        if (m_toolButtonOption->toolButtonStyle != Qt::ToolButtonTextUnderIcon) {
            size.rwidth() += m_indicatorProperties->layout()->width().value_or(0);
        } else {
            size.rheight() += m_indicatorProperties->layout()->height().value_or(0);
        }
    }

    return size;
}

QRectF ToolButtonElement::subControlRect(QStyle::SubControl subControl) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "subControlRect for " << subControl << "is not valid";
        return QRect();
    }

    QRectF backgroundRect = backgroundRectangle(m_toolButtonOption, m_backgroundProperties).toRect();
    if (subControl == QStyle::SC_ToolButton) {
        QRectF rect = m_toolButtonOption->rect;
        QRectF unifiedRect;
        for (const auto &m : m_layoutMap) {
            unifiedRect = unifiedRect.united(m.rect.toRect());
        }
        rect = unifiedRect;
        return rect;
    }
    if (subControl == QStyle::SC_ToolButtonMenu) {
        QRectF menuRect = m_layoutMap[ElementString::Indicator].rect;
        // Set the click area to full height/width, so that its easier to click
        if (m_toolButtonOption->toolButtonStyle != Qt::ToolButtonTextUnderIcon) {
            menuRect.setTop(backgroundRect.top());
            menuRect.setBottom(backgroundRect.bottom());
        } else {
            menuRect.setLeft(backgroundRect.left());
            menuRect.setRight(backgroundRect.right());
        }
        return menuRect;
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

void ToolButtonElement::drawText(QPainter *painter) const
{
    if (m_toolButtonOption->toolButtonStyle == Qt::ToolButtonIconOnly) {
        return;
    }
    AbstractElement::drawText(painter);
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
    if (m_toolButtonOption->features.testFlag(QStyleOptionToolButton::ToolButtonFeature::Menu)) {
        hints.append(u"with-menu"_s);
    }
    if (!m_toolButtonOption->state.testFlag(QStyle::State_AutoRaise)) {
        hints.append(u"raised"_s);
    }
    return hints;
}
