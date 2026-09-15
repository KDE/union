// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ItemViewElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QListView>
#include <QPainter>
#include <QStyle>
#include <QTableView>
#include <QTreeView>

using namespace Qt::StringLiterals;

// ItemViewElements are often used in many weird ways within applications, so we need to ensure they're
// backwards compatible. This means we can't give them custom layouts without breaking everything.
// Instead, we rely on QCommonStyle for most things, but apply our looks to the items.

ItemViewElement::ItemViewElement(const QStyleOptionViewItem *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_viewItemOption(option)
{
    update();
}

ItemViewElement::~ItemViewElement()
{
}

void ItemViewElement::update()
{
    if (m_viewItemOption->features.testFlag(QStyleOptionViewItem::HasCheckIndicator)) {
        m_indicatorElementList = prepareElements(m_styleOption, m_widget, {ElementString::ItemViewItem, ElementString::CheckBox});
        if (!m_indicatorElementList.isEmpty()) {
            m_indicatorProperties = queryProperties(m_indicatorElementList);
        }
    }
    setIcon(m_viewItemOption->icon);
    setText(m_viewItemOption->text);
    layout();
}

void ItemViewElement::layout()
{
    m_backgroundElementList = prepareElements(m_viewItemOption, m_widget, {ElementString::ItemViewItem});

    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);

        auto opt = *m_viewItemOption;
        opt.rect = backgroundRectangle(m_viewItemOption, m_backgroundProperties).toRect();

        m_layoutMap[ElementString::Text].rect = m_style->QCommonStyle::subElementRect(QStyle::SE_ItemViewItemText, &opt, m_widget);
        m_layoutMap[ElementString::Icon].rect = m_style->QCommonStyle::subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, m_widget);
        m_layoutMap[ElementString::CheckBox].rect = m_style->QCommonStyle::subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt, m_widget);

        // Apply our spacing
        switch (m_viewItemOption->decorationPosition) {
        case QStyleOptionViewItem::Left:
            m_layoutMap[ElementString::Text].rect.moveLeft(m_layoutMap[ElementString::Icon].rect.right() + spacing());
            break;
        case QStyleOptionViewItem::Right:
            m_layoutMap[ElementString::Text].rect.moveRight(m_layoutMap[ElementString::Icon].rect.left() - spacing());
            break;
        case QStyleOptionViewItem::Top:
            m_layoutMap[ElementString::Text].rect.moveTop(m_layoutMap[ElementString::Icon].rect.bottom() + spacing());
            break;
        case QStyleOptionViewItem::Bottom:
            m_layoutMap[ElementString::Text].rect.moveBottom(m_layoutMap[ElementString::Icon].rect.top() - spacing());
            break;
        }

        m_isValid = true;
    } else {
        m_isValid = false;
        qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
    }
}

void ItemViewElement::drawIndicator(QPainter *painter) const
{
    // Draw indicator
    if (m_viewItemOption && m_viewItemOption->features.testFlag(QStyleOptionViewItem::HasCheckIndicator)) {
        QStyleOptionButton checkbox;
        switch (m_viewItemOption->checkState) {
        case Qt::Unchecked:
            checkbox.state.setFlag(QStyle::State_Off);
            break;
        case Qt::PartiallyChecked:
            checkbox.state.setFlag(QStyle::State_NoChange);
            break;
        case Qt::Checked:
            checkbox.state.setFlag(QStyle::State_On);
            break;
        }
        checkbox.state.setFlag(QStyle::State_Enabled, m_viewItemOption->state.testFlag(QStyle::State_Enabled));
        checkbox.rect = subElementRect(QStyle::SE_ItemViewItemCheckIndicator).toRect();
        painter->save();
        m_style->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &checkbox, painter);
        painter->restore();
    }
}

QSizeF ItemViewElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    const QSizeF commonSize = m_style->QCommonStyle::sizeFromContents(QStyle::CT_ItemViewItem, m_styleOption, contentsSizeFromStyle.toSize(), m_widget);
    return applyPaddingToSize(commonSize);
}

QRectF ItemViewElement::subElementRect(QStyle::SubElement element) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "Subelementrect for " << element << "is not valid";
        return QRect();
    }

    QRectF rect;
    if (element == QStyle::SE_ItemViewItemText) {
        rect = m_layoutMap[ElementString::Text].rect;
    }
    if (element == QStyle::SE_ItemViewItemDecoration) {
        rect = m_layoutMap[ElementString::Icon].rect;
    }
    if (element == QStyle::SE_ItemViewItemCheckIndicator) {
        rect = m_layoutMap[ElementString::CheckBox].rect;
        // Center the checkbox with its proper size
        rect = centerRect(rect, indicatorSize().width(), indicatorSize().height());
        rect.moveCenter(QPointF(rect.center().x() + spacing() / 2.0, rect.center().y()));
    }

    return rect;
}

void ItemViewElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_ItemViewItem:
        drawBackground(painter);
        drawIcon(painter);
        drawText(painter);
        drawIndicator(painter);
        break;
    }

    switch (enums.PrimitiveElement) {
    case QStyle::PE_PanelItemViewItem:
        drawBackground(painter);
        break;
    }
}

void ItemViewElement::drawText(QPainter *painter) const
{
    if (hasText()) {
        QRectF textRect = m_style->subElementRect(QStyle::SE_ItemViewItemText, m_viewItemOption, m_widget);
        if (textRect.isEmpty()) {
            textRect = m_styleOption->rect;
        }
        drawTextAtRect(painter, m_text, textRect, m_backgroundProperties);
    }
}

void ItemViewElement::drawIcon(QPainter *painter) const
{
    if (hasIcon() && m_isValid) {
        QRect iconRect = m_style->subElementRect(QStyle::SE_ItemViewItemDecoration, m_viewItemOption, m_widget);
        drawIconAtRect(painter, m_icon, iconRect);
    }
}
QVariantMap ItemViewElement::elementAttributes() const
{
    QVariantMap map;
    if (m_viewItemOption->decorationPosition == QStyleOptionViewItem::Top) {
        map[u"display"_s] = QVariant(u"text-below-icon"_s);
    }
    if (m_viewItemOption->decorationPosition == QStyleOptionViewItem::Bottom) {
        map[u"display"_s] = QVariant(u"text-above-icon"_s);
    }
    if (m_viewItemOption->decorationPosition == QStyleOptionViewItem::Left) {
        map[u"display"_s] = QVariant(u"text-after-icon"_s);
    }
    if (m_viewItemOption->decorationPosition == QStyleOptionViewItem::Right) {
        map[u"display"_s] = QVariant(u"text-before-icon"_s);
    }

    auto viewItemPosition = m_viewItemOption->viewItemPosition;

    // Override situations where we have treeview but only select one item
    const auto treeItemView = qobject_cast<const QTreeView *>(m_viewItemOption->widget);
    if (treeItemView && treeItemView->selectionBehavior() != QAbstractItemView::SelectRows) {
        viewItemPosition = QStyleOptionViewItem::OnlyOne;
    }

    switch (viewItemPosition) {
    case QStyleOptionViewItem::Invalid:
        map[u"position"_s] = QVariant(u"invalid"_s);
        break;
    case QStyleOptionViewItem::Beginning:
        map[u"position"_s] = QVariant(u"beginning"_s);
        break;
    case QStyleOptionViewItem::Middle:
        map[u"position"_s] = QVariant(u"middle"_s);
        break;
    case QStyleOptionViewItem::End:
        map[u"position"_s] = QVariant(u"end"_s);
        break;
    case QStyleOptionViewItem::OnlyOne:
        map[u"position"_s] = QVariant(u"onlyone"_s);
        break;
    }
    return map;
}

QStringList ItemViewElement::elementHints() const
{
    QStringList hints;
    const auto table = qobject_cast<const QTableView *>(m_viewItemOption->widget);
    const auto tree = qobject_cast<const QTreeView *>(m_viewItemOption->widget);
    const auto list = qobject_cast<const QListView *>(m_viewItemOption->widget);
    // For tables and such, we just want to select one item.
    if (table) {
        hints.append(u"inside-table"_s);
    }
    if (tree) {
        hints.append(u"inside-tree"_s);
    }
    if (list) {
        hints.append(u"inside-list"_s);
    }

    // These always have hover effect, i think
    hints.append(u"hover-enabled"_s);

    if (m_viewItemOption->features.testFlag(QStyleOptionViewItem::Alternate)) {
        hints.append(u"alternating-colors"_s);
    }

    if (m_viewItemOption->state.testFlag(QStyle::State_Open)) {
        hints.append(u"expanded"_s);
    }
    return hints;
}
