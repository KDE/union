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
        m_indicatorElementList = prepareElements(m_styleOption, m_widget, {ElementString::CheckBox});
        if (!m_indicatorElementList.isEmpty()) {
            m_indicatorProperties = queryProperties(m_indicatorElementList);
        }
    }
    setIcon(QIcon());
    setText(QString());
    if (m_viewItemOption->features.testFlag(QStyleOptionViewItem::HasDecoration)) {
        setIcon(m_viewItemOption->icon);
    }
    if (m_viewItemOption->features.testFlag(QStyleOptionViewItem::HasDisplay)) {
        setText(m_viewItemOption->text);
    }
    updateSubElementList();
    layout();
}

void ItemViewElement::layout()
{
    // Background and content is separate
    m_backgroundElementList = prepareElements(m_viewItemOption, m_widget, {ElementString::ItemViewItem});

    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_layoutMap = layoutMap(m_backgroundElementList, m_viewItemOption, m_subElementList);
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
        checkbox.rect = m_style->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, m_viewItemOption, m_widget);
        painter->save();
        m_style->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &checkbox, painter);
        painter->restore();
    }
}

void ItemViewElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::ItemViewItem);
    if (!m_viewItemOption->icon.isNull()) {
        m_subElementList.append(ElementString::Icon);
    }
    if (!m_viewItemOption->text.isEmpty()) {
        m_subElementList.append(ElementString::Text);
    }
    // As for now Icon and Text are "pseudo" elements, so they are ignored by the hierarchy.
    // Thus calculate CheckBox last in the layouter, otherwise the icon would become its child
    if (m_viewItemOption->features.testFlag(QStyleOptionViewItem::HasCheckIndicator)) {
        m_subElementList.append(ElementString::CheckBox);
    }
}

QSizeF ItemViewElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    const auto textSize = subElementRect(QStyle::SE_ItemViewItemText).size();
    const auto decorationSize = subElementRect(QStyle::SE_ItemViewItemDecoration).size();
    const auto checkboxSize = subElementRect(QStyle::SE_ItemViewItemCheckIndicator).size();
    const auto combinedSize = QSizeF(std::max({textSize.width(), decorationSize.width(), checkboxSize.width()}),
                                     std::max({textSize.height(), decorationSize.height(), checkboxSize.height()}));
    return contentsSizeFromStyle.expandedTo(applyPaddingToSize(combinedSize));
}

QRectF ItemViewElement::subElementRect(QStyle::SubElement element) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "Subelementrect for " << element << "is not valid";
        return QRect();
    }

    if (m_subElementList.isEmpty()) {
        return QRect();
    }

    QRectF rect;
    if (element == QStyle::SE_ItemViewItemText) {
        rect = m_layoutMap[ElementString::Text].rect;
    }
    if (element == QStyle::SE_ItemViewItemDecoration) {
        // DecorationSize can be changed by user, so use it by default
        rect = m_layoutMap[ElementString::Icon].rect;
    }
    if (element == QStyle::SE_ItemViewItemCheckIndicator) {
        rect = m_layoutMap[ElementString::CheckBox].rect;
    }
    // Ensure the item is centered within the itemview for compatibility reasons:
    // This may stop layouting items to top/bottom instead of center, but readability is more important.
    rect.moveCenter(QPointF(rect.center().x(), m_styleOption->rect.center().y()));
    return m_style->visualRect(m_styleOption->direction, m_styleOption->rect, rect.toRect());
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
    if (hasText() && m_isValid) {
        QRectF textRect = m_style->subElementRect(QStyle::SE_ItemViewItemText, m_viewItemOption, m_widget);
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
        map[u"display"_s] = QVariant(u"text-above-icon"_s);
    }
    if (m_viewItemOption->decorationPosition == QStyleOptionViewItem::Bottom) {
        map[u"display"_s] = QVariant(u"text-below-icon"_s);
    }
    if (m_viewItemOption->decorationPosition == QStyleOptionViewItem::Left) {
        map[u"display"_s] = QVariant(u"text-after-icon"_s);
    }
    if (m_viewItemOption->decorationPosition == QStyleOptionViewItem::Right) {
        map[u"display"_s] = QVariant(u"text-before-icon"_s);
    }

    auto viewItemPosition = m_viewItemOption->viewItemPosition;
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

    if (m_viewItemOption->state.testFlag(QStyle::State_Open)) {
        hints.append(u"expanded"_s);
    }
    return hints;
}
