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

TabElement::TabElement(const QStyleOptionTab *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_tabOption(option)
    , m_isVertical(false)
    , m_isClosable(false)
{
    update();
}

TabElement::~TabElement()
{
}

void TabElement::update()
{
    m_isVertical = m_tabOption->shape == QTabBar::RoundedEast || m_tabOption->shape == QTabBar::RoundedWest || m_tabOption->shape == QTabBar::TriangularEast
        || m_tabOption->shape == QTabBar::TriangularWest;

    if (const auto tabbarwidget = qobject_cast<const QTabBar *>(m_widget)) {
        if (tabbarwidget->tabsClosable()) {
            m_isClosable = true;
        }
    }

    setIcon(m_tabOption->icon);
    setText(m_tabOption->text);
    updateSubElementList();
    layout();
}

void TabElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
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
    if (m_isClosable) {
        m_subElementList.append(ElementString::CloseButton);
    }
    if (!m_tabOption->icon.isNull()) {
        m_subElementList.append(ElementString::Icon);
    }
    if (!m_tabOption->text.isEmpty()) {
        m_subElementList.append(ElementString::Text);
    }
}

void TabElement::layout()
{
    if (m_subElementList.isEmpty()) {
        m_isValid = false;
        return;
    }

    m_backgroundElementList = prepareElements(m_tabOption, m_widget, {ElementString::Tab});
    if (!m_backgroundElementList.isEmpty()) {
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
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_layoutMap = layoutMap(m_backgroundElementList, &subopt, m_subElementList);
        m_isValid = true;
    }
}

QSizeF TabElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    // Follow what Breeze does here

    const bool leftButton = !m_tabOption->leftButtonSize.isEmpty();
    const bool rightButton = !m_tabOption->leftButtonSize.isEmpty();
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
    QSizeF size(applyPaddingToSize(contentsSizeFromStyle));

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
    if (!m_isValid) {
        return QRect();
    }

    if (element == QStyle::SE_TabBarTabText) {
        QRectF unifiedRect;
        for (const auto &m : m_layoutMap) {
            unifiedRect = unifiedRect.united(m.rect.toRect());
        }
        unifiedRect.setSize(applyPaddingToSize(unifiedRect.size()));
        if (m_isVertical) {
            unifiedRect = unifiedRect.transposed();
        }
    }
    return m_tabOption->rect;
}

bool TabElement::isVertical() const
{
    return m_isVertical;
}

// Padding of the tab content and the edge, only one value is taken so take the largest one
int TabElement::hSpace() const
{
    if (m_isValid && m_backgroundProperties && m_backgroundProperties->layout() && m_backgroundProperties->layout()->padding()) {
        auto padding = m_backgroundProperties->layout()->padding()->toMargins();
        return qMax(padding.left(), padding.right());
    }
    return 0;
}

int TabElement::vSpace() const
{
    if (m_isValid && m_backgroundProperties && m_backgroundProperties->layout() && m_backgroundProperties->layout()->padding()) {
        auto padding = m_backgroundProperties->layout()->padding()->toMargins();
        return qMax(padding.top(), padding.bottom());
    }
    return 0;
}

QVariantMap TabElement::elementAttributes() const
{
    QVariantMap map;
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
    return QStringList();
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
