// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "LineEditElement.h"
#include "StyleUtils.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>
#include <QTableWidget>

#include "SharedNames.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;

LineEditElement::LineEditElement(const QStyleOptionFrame *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_frameOption(option)
{
    update();
}

LineEditElement::~LineEditElement()
{
}

void LineEditElement::update()
{
    updateSubElementList();
    layout();
}

void LineEditElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    // For spinboxes and comboboxes, we do not want to draw this element
    // TODO: maybe this should be handleable by the CSS
    if (!m_widget || m_widget->parentWidget()->inherits("QComboBox") || m_widget->parentWidget()->inherits("QAbstractSpinBox")) {
        return;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_PanelLineEdit:
        drawBackground(painter);
        break;
    case QStyle::PE_FrameLineEdit:
        drawFrame(painter);
        break;
    }
}

QSizeF LineEditElement::iconSize() const
{
    return querySize({ElementString::LineEditIconSize});
}

void LineEditElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::TextField);
}

QSizeF LineEditElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    // LineEdit wants much more simplified size in Widgets, we ignore the width/height set in CSS.
    QMarginsF padding =
        safePropertyLookup(m_backgroundProperties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
    QMarginsF inset =
        safePropertyLookup(m_backgroundProperties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::inset, &SizePropertyGroup::toMargins);
    padding += inset;
    return contentsSizeFromStyle.grownBy(padding);
}

QMarginsF LineEditElement::iconPadding() const
{
    if (!m_isValid) {
        return QMarginsF();
    }
    return safePropertyLookup(m_contentProperties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::inset, &SizePropertyGroup::toMargins);
}

QRectF LineEditElement::subElementRect(QStyle::SubElement element) const
{
    if (m_isValid && element == QStyle::SE_LineEditContents && m_backgroundProperties && m_backgroundProperties->layout()) {
        int frameWidth = m_style->pixelMetric(QStyle::PM_DefaultFrameWidth, m_frameOption, m_widget);
        return backgroundRectangle(m_frameOption, m_backgroundProperties).toRect().adjusted(frameWidth, frameWidth, -frameWidth, -frameWidth);
    }
    return QRectF();
}

QStringList LineEditElement::elementHints() const
{
    auto hints = frameHints(m_frameOption);
    // Due to how Line Edits work, we have to first check the parent, then it's parent
    // to figure out if we're inside a table. The first parent is just the scrollview.
    if (m_widget && m_widget->parentWidget()) {
        if (auto realParent = m_widget->parentWidget()->parentWidget()) {
            if (realParent->inherits("QTableView") || realParent->inherits("QTableWidget")) {
                hints.append(u"inside-table"_s);
            }
        }
    }
    return hints;
}

qreal LineEditElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_LineEditIconMargin: {
        const auto margins = iconPadding();
        return (margins.left() + margins.right() + margins.top() + margins.bottom()) / 4;
    }
    case QStyle::PM_LineEditIconSize:
        return iconSize().width();
    default:
        break;
    }
    return 0;
}
