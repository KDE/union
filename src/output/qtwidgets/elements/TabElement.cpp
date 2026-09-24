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

TabElement::TabElement(const QStyleOptionTab *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_tabOption(option)
    , m_isVertical(false)
    , m_isStatic(false)
{
    update();
}

TabElement::~TabElement()
{
}

void TabElement::update()
{
    if (!m_tabOption) {
        return;
    }
    m_isVertical = m_tabOption->shape == QTabBar::RoundedEast || m_tabOption->shape == QTabBar::RoundedWest || m_tabOption->shape == QTabBar::TriangularEast
        || m_tabOption->shape == QTabBar::TriangularWest;

    if (const auto tabBar = qobject_cast<const QTabBar *>(m_widget)) {
        m_isStatic = m_tabOption->documentMode && tabBar && !tabBar->tabsClosable() && !tabBar->isMovable() && (tabBar->expanding() || m_isVertical);
    } else {
        m_isStatic = false;
    }

    setIcon(m_tabOption->icon);
    setText(m_tabOption->text);
    updateSubElementList();
    layout();
}

void TabElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid || !m_tabOption) {
        return;
    }

    // Rotate the painter if needed, remember to restore after
    auto saveAndRotatePainter = [this](QPainter *painter) {
        painter->save();
        QRect tabRect = m_tabOption->rect;
        if (m_isVertical) {
            int newX, newY, newRot;
            if (m_tabOption->shape == QTabBar::RoundedEast || m_tabOption->shape == QTabBar::TriangularEast) {
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

void TabElement::updateSubElementList()
{
    m_subElementList.clear();
    if (!m_tabOption->icon.isNull()) {
        m_subElementList.append(ElementString::Icon);
    }
    if (!m_tabOption->text.isEmpty()) {
        m_subElementList.append(ElementString::Text);
    }
}

void TabElement::layout()
{
    if (m_subElementList.isEmpty() || !m_tabOption) {
        m_isValid = false;
        return;
    }

    m_backgroundElementList = prepareElements(m_tabOption, m_widget, {ElementString::Tab});
    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        QRect tabRect = m_tabOption->rect;
        // Reset the coordinates for vertical tabs
        if (m_isVertical) {
            tabRect.setRect(0, 0, tabRect.height(), tabRect.width());
        }

        // Leave space for the left and right button
        if (!m_tabOption->leftButtonSize.isEmpty()) {
            tabRect.setLeft(tabRect.left() + spacing() + (m_isVertical ? m_tabOption->leftButtonSize.height() : m_tabOption->leftButtonSize.width()));
        }
        if (!m_tabOption->rightButtonSize.isEmpty()) {
            tabRect.setRight(tabRect.right() - spacing() - (m_isVertical ? m_tabOption->rightButtonSize.height() : m_tabOption->rightButtonSize.width()));
        }

        auto subopt = *m_tabOption;
        subopt.rect = tabRect;
        m_layoutMap = layoutMap(m_backgroundElementList, &subopt, m_subElementList);
        m_isValid = true;
    }
}

QSizeF TabElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    // Follow what Breeze does here
    if (!m_isValid || !m_tabOption) {
        return contentsSizeFromStyle;
    }

    const bool leftButton = !m_tabOption->leftButtonSize.isEmpty();
    const bool rightButton = !m_tabOption->rightButtonSize.isEmpty();
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

    QMarginsF padding =
        safePropertyLookup(m_backgroundProperties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);

    // contentSizeFromStyle includes hSpace and vSpace for which we return an
    // average. Subtract that so applyPaddingToSize is handled correctly.
    auto size = QSizeF{
        contentsSizeFromStyle.width() - (padding.left() + padding.right()) / 2,
        contentsSizeFromStyle.height() - (padding.top() + padding.bottom()) / 2,
    };

    // add margins
    size = applyPaddingToSize(size);

    if (m_isVertical) {
        size.rheight() += widthIncrement;
    } else {
        size.rwidth() += widthIncrement;
    }
    size = size.expandedTo(QSize(width(), height()));

    return size;
}

QRectF TabElement::subElementRect(QStyle::SubElement element) const
{
    return m_style->QCommonStyle::subElementRect(element, m_styleOption, m_widget);
}

// Padding of the tab content and the edge, only one value is taken so take the average
int TabElement::hSpace() const
{
    if (m_isValid) {
        auto padding =
            safePropertyLookup(m_backgroundProperties, QMarginsF(), &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
        return (padding.left() + padding.right()) / 2;
    }
    return 0;
}

int TabElement::vSpace() const
{
    if (m_isValid) {
        auto padding =
            safePropertyLookup(m_backgroundProperties, QMarginsF(), &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
        return (padding.top() + padding.bottom()) / 2;
    }
    return 0;
}

QVariantMap TabElement::elementAttributes() const
{
    QVariantMap map;
    if (!m_tabOption) {
        return map;
    }
    const bool top = m_tabOption->shape == QTabBar::RoundedNorth || m_tabOption->shape == QTabBar::TriangularNorth;
    const bool bottom = m_tabOption->shape == QTabBar::RoundedSouth || m_tabOption->shape == QTabBar::TriangularSouth;
    const bool left = m_tabOption->shape == QTabBar::RoundedWest || m_tabOption->shape == QTabBar::TriangularWest;
    const bool right = m_tabOption->shape == QTabBar::RoundedEast || m_tabOption->shape == QTabBar::TriangularEast;

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

QStringList TabElement::elementHints() const
{
    QStringList hints;

    if (m_isStatic) {
        hints.append(u"immutable"_s);
    }

    auto tabStyleOption = qstyleoption_cast<const QStyleOptionTab *>(m_styleOption);
    auto tabBar = qobject_cast<const QTabBar *>(m_widget);
    if (tabStyleOption && tabBar) {
        if (tabStyleOption->tabIndex == 0) {
            hints.append(u"first"_s);
        } else if (tabStyleOption->tabIndex == tabBar->count() - 1) {
            hints.append(u"last"_s);
        }
    }

    return hints;
}

qreal TabElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_TabCloseIndicatorWidth:
        return indicatorSize().width();
    case QStyle::PM_TabCloseIndicatorHeight:
        return indicatorSize().height();
    case QStyle::PM_TabBarTabHSpace:
        return hSpace();
    case QStyle::PM_TabBarTabVSpace:
        return vSpace();
    default:
        break;
    }
    return 0;
}
