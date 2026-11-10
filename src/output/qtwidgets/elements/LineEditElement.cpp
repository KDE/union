// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "LineEditElement.h"
#include "FrameElement.h"
#include "SharedNames.h"
#include "StyleUtils.h"
#include "UnionPainter.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>
#include <QTableWidget>

using namespace Qt::StringLiterals;
using namespace Union::Properties;

LineEditElement::LineEditElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_hasFrame(true)
{
    update(option);
}

LineEditElement::~LineEditElement()
{
}

void LineEditElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    if (const auto newOption = qstyleoption_cast<const QStyleOptionFrame *>(option)) {
        m_frameOption = newOption;
    }
    if (const auto lineEdit = qobject_cast<const QLineEdit *>(m_widget)) {
        m_hasFrame = lineEdit->hasFrame();
    } else if (!m_widget) {
        // Widget-less callers indicate the frame through lineWidth, not frameShape.
        m_hasFrame = m_frameOption && m_frameOption->lineWidth > 0;
    } else {
        m_hasFrame = m_frameOption && m_frameOption->frameShape != QFrame::Shape::NoFrame;
    }

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
    if (m_widget && m_widget->parentWidget()) {
        const auto parent = m_widget->parentWidget();
        if (parent->inherits("QComboBox") || parent->inherits("QAbstractSpinBox")) {
            return;
        }
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

void LineEditElement::drawBackground(QPainter *painter) const
{
    if (m_isValid && m_styleOption) {
        QColor backgroundOverride;
        const auto &palette = m_styleOption->palette;
        if (palette.isBrushSet(palette.currentColorGroup(), QPalette::Base)) {
            backgroundOverride = palette.base().color();
        }

        UnionPainter unionPainter(painter, m_elementProperties);
        unionPainter.setParts(m_hasFrame ? UnionPainter::Parts::All : UnionPainter::Parts::PanelOnly);
        unionPainter.setOverrideBackgroundColor(backgroundOverride);
        unionPainter.setRect(m_styleOption->rect);
        unionPainter.draw();
    }
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
        safePropertyLookup(m_elementProperties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
    return contentsSizeFromStyle.grownBy(padding);
}

QMarginsF LineEditElement::iconPadding() const
{
    if (!m_isValid) {
        return QMarginsF();
    }
    return safePropertyLookup(m_elementProperties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
}

QRectF LineEditElement::subElementRect(QStyle::SubElement element) const
{
    if (m_isValid && m_frameOption && element == QStyle::SE_LineEditContents) {
        auto rect = backgroundRectangle(m_frameOption, m_elementProperties).toRect();
        return rect.marginsRemoved(padding().toMargins());
    }
    return m_style->QCommonStyle::subElementRect(element, m_styleOption, m_widget);
}

Union::IdentifierVariantHash LineEditElement::elementAttributes() const
{
    return FrameElement::frameAttributesFor(m_frameOption, m_widget);
}

Union::IdentifierList LineEditElement::elementHints() const
{
    auto hints = FrameElement::frameHintsFor(m_frameOption);
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
        return querySize({ElementString::LineEditIconSize}).width();
    default:
        break;
    }
    return 0;
}

QString LineEditElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
{
    return ElementString::TextField;
}
