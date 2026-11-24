// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ItemViewElement.h"

#include <QListView>
#include <QTableView>
#include <QTreeView>

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

// ItemViewElements are often used in many weird ways within applications, so we need to ensure they're
// backwards compatible. This means we can't give them custom layouts without breaking everything.
// Instead, we rely on QCommonStyle for most things, but apply our looks to the items.

ItemViewElement::ItemViewElement(const UnionStyle *style, const QWidget *widget)
    : AbstractElement(style, widget)
{
}

void ItemViewElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    if (const auto newOpt = qstyleoption_cast<const QStyleOptionViewItem *>(option)) {
        m_viewItemOption = newOpt;
    }
    if (m_viewItemOption) {
        if (m_viewItemOption->features.testFlag(QStyleOptionViewItem::HasCheckIndicator)) {
            m_indicatorElementList = prepareElements(option, widget(), {ElementString::ItemViewItem, ElementString::CheckBox});
            if (!m_indicatorElementList.isEmpty()) {
                m_indicatorProperties = queryProperties(m_indicatorElementList);
            }
        }
        setIcon(m_viewItemOption->icon);
        setText(m_viewItemOption->text);
    }
    layout();
}

void ItemViewElement::layout()
{
    m_elementList = prepareElements(option, widget(), {ElementString::ItemViewItem});
    if (!m_elementList.isEmpty()) {
        m_elementProperties = queryProperties(m_elementList);
        if (m_viewItemOption) {
            auto opt = *m_viewItemOption;
            opt.rect = visualRect(backgroundRectangle(m_viewItemOption, m_elementProperties).toRect()).toRect();
            m_layoutMap[ElementString::Text].rect = style()->QCommonStyle::subElementRect(QStyle::SE_ItemViewItemText, &opt, widget());
            m_layoutMap[ElementString::Icon].rect = style()->QCommonStyle::subElementRect(QStyle::SE_ItemViewItemDecoration, &opt, widget());
            m_layoutMap[ElementString::CheckBox].rect =
                visualRect(style()->QCommonStyle::subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt, widget()));

            // Apply our spacing
            auto textRect = visualRect(m_layoutMap[ElementString::Text].rect);
            const auto iconRect = visualRect(m_layoutMap[ElementString::Icon].rect);
            switch (m_viewItemOption->decorationPosition) {
            case QStyleOptionViewItem::Left:
                textRect.moveLeft(iconRect.right() + spacing());
                break;
            case QStyleOptionViewItem::Right:
                textRect.moveRight(iconRect.left() - spacing());
                break;
            case QStyleOptionViewItem::Top:
                textRect.moveTop(iconRect.bottom() + spacing());
                break;
            case QStyleOptionViewItem::Bottom:
                textRect.moveBottom(iconRect.top() - spacing());
                break;
            }
        } else {
            // Fallback
            m_layoutMap[ElementString::Text].rect = style()->QCommonStyle::subElementRect(QStyle::SE_ItemViewItemText, option, widget());
            m_layoutMap[ElementString::Icon].rect = style()->QCommonStyle::subElementRect(QStyle::SE_ItemViewItemDecoration, option, widget());
            m_layoutMap[ElementString::CheckBox].rect = style()->QCommonStyle::subElementRect(QStyle::SE_ItemViewItemCheckIndicator, option, widget());
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
        style()->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &checkbox, painter);
        painter->restore();
    }
}

QSizeF ItemViewElement::contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const
{
    const QSizeF commonSize = style()->QCommonStyle::sizeFromContents(QStyle::CT_ItemViewItem, option, contentsSizeFromStyle.toSize(), widget());
    return applyPaddingToSize(commonSize);
}

QRectF ItemViewElement::subElementRect(const QStyleOption *option, QStyle::SubElement element) const
{
    if (m_isValid) {
        QRectF rect;
        if (element == QStyle::SE_ItemViewItemText) {
            const auto adjustment = spacing() / 2.0;
            rect = m_layoutMap[ElementString::Text].rect.adjusted(adjustment, 0, -adjustment, 0);
        }
        if (element == QStyle::SE_ItemViewItemDecoration) {
            rect = m_layoutMap[ElementString::Icon].rect;
        }
        if (element == QStyle::SE_ItemViewItemCheckIndicator) {
            rect = m_layoutMap[ElementString::CheckBox].rect;
            // Center the checkbox with its proper size
            rect = centerRect(rect, width(m_indicatorProperties), height(m_indicatorProperties));
            rect.moveCenter(QPointF(rect.center().x() + spacing() / 2.0, rect.center().y()));
        }
        return visualRect(rect);
    }
    return style()->QCommonStyle::subElementRect(element, option, widget());
}

void ItemViewElement::draw(QPainter *painter, DrawVariant what) const
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

void ItemViewElement::drawBackground(QPainter *painter) const
{
    if (!option) {
        return;
    }
    painter->save();
    // Apply clip region in case the user has set one for painter.
    const QRect rect = option->rect;
    const QRegion clipRegion = painter->hasClipping() ? (painter->clipRegion() & rect) : rect;
    painter->setClipRegion(clipRegion);

    // Draw custom background color with simple rectangle if set and not selected
    bool hasCustomBackground = false;
    if (m_viewItemOption) {
        hasCustomBackground = m_viewItemOption->backgroundBrush.style() != Qt::NoBrush && !option->state.testFlag(QStyle::State_Selected);
    }
    if (hasCustomBackground) {
        const auto radius =
            safePropertyLookup(m_elementProperties, CornersPropertyGroup::CornerRadii(), &StylePropertyGroup::corners, &CornersPropertyGroup::radii);

        const auto path = UnionPainter::unevenRadiiRectPath(rect, radius);
        painter->setPen(Qt::NoPen);
        painter->setBrush(m_viewItemOption->backgroundBrush);
        painter->drawPath(path);
    } else {
        QColor overrideColor;
        UnionPainter::Parts parts = UnionPainter::Parts::All;
        // Ignore borders in these cases, as they will use different color from style, and we have no border color in qpalette
        if (option) {
            // Either set different highlight or alternateBase color if they're set
            const bool selected = option->state.testFlag(QStyle::State_Selected);
            const auto palette = option->palette;
            if (selected && palette.isBrushSet(palette.currentColorGroup(), QPalette::Highlight)) {
                overrideColor = option->palette.highlight().color();
                parts = UnionPainter::Parts::PanelOnly;
            }
            if (!selected && palette.isBrushSet(palette.currentColorGroup(), QPalette::AlternateBase)) {
                overrideColor = option->palette.alternateBase().color();
                parts = UnionPainter::Parts::PanelOnly;
            }
        }
        if (m_viewItemOption && m_viewItemOption->backgroundBrush.style() != Qt::NoBrush) {
            overrideColor = m_viewItemOption->backgroundBrush.color();
            parts = UnionPainter::Parts::PanelOnly;
        }
        UnionPainter unionPainter(painter, m_elementProperties);
        unionPainter.setOverrideBackgroundColor(overrideColor);
        unionPainter.setParts(parts);
        unionPainter.setRect(option->rect);
        unionPainter.draw();
    }
    painter->restore();
}

void ItemViewElement::drawText(QPainter *painter) const
{
    if (hasText()) {
        QRectF textRect = style()->subElementRect(QStyle::SE_ItemViewItemText, m_viewItemOption, widget());
        if (textRect.isEmpty()) {
            textRect = option->rect;
        }
        drawTextAtRect(painter, m_text, textRect);
    }
}

void ItemViewElement::drawIcon(QPainter *painter) const
{
    if (hasIcon() && m_isValid) {
        auto iconRect = subElementRect(QStyle::SE_ItemViewItemDecoration);
        drawIconAtRect(painter, m_icon, iconRect);
    }
}
Union::IdentifierVariantHash ItemViewElement::elementAttributes() const
{
    Union::IdentifierVariantHash map;
    if (m_viewItemOption) {
        if (m_viewItemOption->decorationPosition == QStyleOptionViewItem::Top) {
            map[u"display"_s] = QVariant(u"text-below-icon"_s);
        }
        if (m_viewItemOption->decorationPosition == QStyleOptionViewItem::Bottom) {
            map[u"display"_s] = QVariant(u"text-above-icon"_s);
        }
        if (m_viewItemOption->decorationPosition == QStyleOptionViewItem::Left) {
            map[u"display"_s] = QVariant(u"text-beside-icon"_s);
        }
        if (m_viewItemOption->decorationPosition == QStyleOptionViewItem::Right) {
            map[u"display"_s] = QVariant(u"text-before-icon"_s);
        }

        auto viewItemPosition = m_viewItemOption->viewItemPosition;

        // Override situations where we have treeview but only select one item
        const auto abstractItemView = qobject_cast<const QAbstractItemView *>(m_viewItemOption->widget);
        if (abstractItemView && abstractItemView->selectionBehavior() != QAbstractItemView::SelectRows) {
            viewItemPosition = QStyleOptionViewItem::OnlyOne;
        }

        switch (viewItemPosition) {
        case QStyleOptionViewItem::Invalid:
            map[u"position"_s] = QVariant(u"invalid"_s);
            break;
        case QStyleOptionViewItem::Beginning:
            map[u"position"_s] = m_viewItemOption->direction == Qt::RightToLeft ? QVariant(u"end"_s) : QVariant(u"beginning"_s);
            break;
        case QStyleOptionViewItem::Middle:
            map[u"position"_s] = QVariant(u"middle"_s);
            break;
        case QStyleOptionViewItem::End:
            map[u"position"_s] = m_viewItemOption->direction == Qt::RightToLeft ? QVariant(u"beginning"_s) : QVariant(u"end"_s);
            break;
        case QStyleOptionViewItem::OnlyOne:
            map[u"position"_s] = QVariant(u"onlyone"_s);
            break;
        }
    } else {
        map[u"display"_s] = QVariant(u"text-beside-icon"_s);
        map[u"position"_s] = QVariant(u"invalid"_s);
    }
    return map;
}

Union::IdentifierList ItemViewElement::elementHints() const
{
    Union::IdentifierList hints;
    if (m_viewItemOption) {
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

        // For combobox popups, we want similar selector as QtQuick has,
        // (popup.combobox) so we need to insert the combobox hint here.
        if (widget() && widget()->inherits("QComboBoxListView")) {
            hints.append(u"combobox"_s);
        }
    }

    return hints;
}

QString ItemViewElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
{
    return ElementString::ItemViewItem;
}
