// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "TabElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>
#include <QTabBar>

using namespace Qt::StringLiterals;
using namespace Union::Properties;

TabElement::TabElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_isVertical(false)
    , m_isStatic(false)
    , m_shape(QTabBar::Shape::RoundedNorth)
    , m_leftButtonSize(QSize())
    , m_rightButtonSize(QSize())
{
    setFlag(AbstractElementFlag::ContentsSizeIncludesAveragePadding);
    setFlag(AbstractElementFlag::DrawMnemonics);
    update(option);
}

TabElement::~TabElement()
{
}

void TabElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    if (const auto newOpt = qstyleoption_cast<const QStyleOptionTab *>(option)) {
        m_tabOption = newOpt;
    }

    if (m_tabOption) {
        m_shape = m_tabOption->shape;
        m_isVertical = m_tabOption->shape == QTabBar::RoundedEast || m_tabOption->shape == QTabBar::RoundedWest || m_tabOption->shape == QTabBar::TriangularEast
            || m_tabOption->shape == QTabBar::TriangularWest;

        if (const auto tabBar = qobject_cast<const QTabBar *>(m_widget)) {
            m_isStatic = m_tabOption->documentMode && tabBar && !tabBar->tabsClosable() && !tabBar->isMovable() && (tabBar->expanding() || m_isVertical);
        } else {
            m_isStatic = false;
        }

        setIcon(m_tabOption->icon);
        setText(m_tabOption->text);

        m_leftButtonSize = m_tabOption->leftButtonSize;
        m_rightButtonSize = m_tabOption->rightButtonSize;
    }

    updateSubElementList();
    layout();
    if (m_styleOption) {
        updateGraphicsEffect(m_styleOption->rect);
    }
}

void TabElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid || !m_styleOption) {
        return;
    }

    // Rotate the painter if needed, remember to restore after
    // TODO: Tab buttons have some issues with shadows on vertical tabs
    // The effect painter gets upset at saveAndRotatePainter
    auto saveAndRotatePainter = [this](QPainter *painter) {
        painter->save();
        QRect tabRect = m_styleOption->rect;
        if (m_isVertical) {
            int newX, newY, newRot;
            if (m_shape == QTabBar::RoundedEast || m_shape == QTabBar::TriangularEast) {
                newX = tabRect.width() + tabRect.x();
                newY = tabRect.y();
                newRot = 90;
            } else {
                newX = tabRect.x();
                newY = tabRect.y() + tabRect.height();
                newRot = -90;
            }
            QTransform m = QTransform::fromTranslate(newX, newY);
            m.rotate(newRot);
            painter->setTransform(m, true);
        }
    };

    switch (enums.ControlElement) {
    case QStyle::CE_TabBarTab:
        drawBackground(painter);
        saveAndRotatePainter(painter);
        drawIcon(painter);
        drawText(painter);
        drawIndicator(painter);
        painter->restore();
        break;
    case QStyle::CE_TabBarTabShape:
        drawBackground(painter);
        break;
    case QStyle::CE_TabBarTabLabel:
        saveAndRotatePainter(painter);
        drawIcon(painter);
        drawText(painter);
        painter->restore();
        break;
    }
}

void TabElement::drawText(QPainter *painter) const
{
    if (hasText() && m_isValid) {
        auto textContainer = m_style->subElementRect(QStyle::SE_TabBarTabText, m_styleOption, m_widget);
        auto alignment = m_layoutMap[ElementString::Text].horizontalAlignment;
        auto textRect = m_layoutMap[ElementString::Text].rect;

        switch (alignment) {
        case Alignment::Unspecified:
        case Alignment::Start:
            textRect.moveLeft(textContainer.left());
            break;
        case Alignment::End:
            textRect.moveRight(textContainer.right());
            break;
        case Alignment::Center:
        case Alignment::Fill:
        case Alignment::StackCenter:
        case Alignment::StackFill:
            textRect.moveCenter(textContainer.center());
            break;
        }
        drawTextAtRect(painter, m_text, textRect);
    }
}

void TabElement::updateSubElementList()
{
    m_subElementList.clear();
    if (hasIcon()) {
        m_subElementList.append(ElementString::Icon);
    }
    if (hasText()) {
        m_subElementList.append(ElementString::Text);
    }
}

void TabElement::layout()
{
    if (m_subElementList.isEmpty() || !m_styleOption) {
        m_isValid = false;
        return;
    }

    m_elementList = prepareElements(m_styleOption, m_widget, {ElementString::Tab});
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        QRect tabRect = m_styleOption->rect;
        // Reset the coordinates for vertical tabs
        if (m_isVertical) {
            tabRect.setRect(0, 0, tabRect.height(), tabRect.width());
        }

        // Leave space for the left and right button{
        if (!m_rightButtonSize.isEmpty()) {
            tabRect.setLeft(tabRect.left() + spacing() + (m_isVertical ? m_leftButtonSize.height() : m_leftButtonSize.width()));
        }
        if (!m_rightButtonSize.isEmpty()) {
            tabRect.setRight(tabRect.right() - spacing() - (m_isVertical ? m_rightButtonSize.height() : m_rightButtonSize.width()));
        }

        // Needed for positioning the text and icon properly
        if (m_tabOption) {
            auto subopt = *m_tabOption;
            subopt.rect = tabRect;
            m_layoutMap = layoutMap(&subopt, m_subElementList);
        } else {
            m_layoutMap = layoutMap(m_styleOption, m_subElementList);
        }
        m_isValid = true;
    } else {
        m_isValid = false;
    }
}

QSizeF TabElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    // Follow what Breeze does here
    if (m_isValid && m_styleOption) {
        const bool leftButton = !m_leftButtonSize.isEmpty();
        const bool rightButton = !m_rightButtonSize.isEmpty();
        const bool text = hasText();
        const bool icon = hasIcon();
        const qreal offset = spacing();

        // calculate width increment for horizontal tabs
        int widthIncrement = 0;
        if (icon && !(text || leftButton || rightButton)) {
            widthIncrement -= offset;
        }
        if (text && icon) {
            widthIncrement += offset;
        }
        if (leftButton && (text || icon)) {
            widthIncrement += offset;
        }
        if (rightButton && (text || icon || leftButton)) {
            widthIncrement += offset;
        }

        // add margins
        auto size = contentsSizeFromStyle;

        if (m_isVertical) {
            size.rheight() += widthIncrement;
        } else {
            size.rwidth() += widthIncrement;
        }
        size = size.expandedTo(propertySize());

        return applyPaddingToSize(size);
    }

    return contentsSizeFromStyle;
}

QRectF TabElement::subElementRect(QStyle::SubElement element) const
{
    return m_style->QCommonStyle::subElementRect(element, m_styleOption, m_widget);
}

// Padding of the tab content and the edge, only one value is taken so take the average
int TabElement::hSpace() const
{
    if (m_isValid) {
        auto pad = padding();
        return (pad.left() + pad.right()) / 2;
    }
    return 0;
}

int TabElement::vSpace() const
{
    if (m_isValid) {
        auto pad = padding();
        return (pad.top() + pad.bottom()) / 2;
    }
    return 0;
}

Union::Element::States TabElement::elementStates() const
{
    auto states = AbstractElement::elementStates();
    states.setFlag(Union::Element::State::Highlighted, false);
    states.setFlag(Union::Element::State::Checked, m_styleOption->state.testFlag(QStyle::State_Selected));
    return states;
}

Union::IdentifierVariantHash TabElement::elementAttributes() const
{
    Union::IdentifierVariantHash map;

    const bool top = m_shape == QTabBar::RoundedNorth || m_shape == QTabBar::TriangularNorth;
    const bool bottom = m_shape == QTabBar::RoundedSouth || m_shape == QTabBar::TriangularSouth;
    const bool left = m_shape == QTabBar::RoundedWest || m_shape == QTabBar::TriangularWest;
    const bool right = m_shape == QTabBar::RoundedEast || m_shape == QTabBar::TriangularEast;

    if (top) {
        map[u"direction"_s] = QVariant(u"top"_s);
    }
    if (bottom) {
        map[u"direction"_s] = QVariant(u"bottom"_s);
    }
    if (left) {
        map[u"direction"_s] = QVariant(u"left"_s);
    }
    if (right) {
        map[u"direction"_s] = QVariant(u"right"_s);
    }

    if (hasIcon() && !hasText()) {
        map[u"display"_s] = QVariant(u"icon-only"_s);
    }
    if (!hasIcon() && hasText()) {
        map[u"display"_s] = QVariant(u"text-only"_s);
    }

    return map;
}

Union::IdentifierList TabElement::elementHints() const
{
    Union::IdentifierList hints;
    if (m_isStatic) {
        hints.append(u"immutable"_s);
    }
    const auto tabBar = qobject_cast<const QTabBar *>(m_widget);
    if (m_tabOption && tabBar) {
        if (m_tabOption->tabIndex == 0) {
            hints.append(u"first"_s);
        } else if (m_tabOption->tabIndex == tabBar->count() - 1) {
            hints.append(u"last"_s);
        }
    }
    return hints;
}

qreal TabElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_TabBarTabHSpace:
        return hSpace();
    case QStyle::PM_TabBarTabVSpace:
        return vSpace();
    default:
        break;
    }
    return 0;
}

QString TabElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
{
    return ElementString::Tab;
}
