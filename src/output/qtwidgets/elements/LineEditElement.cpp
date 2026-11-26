// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "LineEditElement.h"

#include <QLineEdit>

#include "FrameElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

static WidgetToElementRegistration<LineEditElement, QLineEdit> lineEditWidgetRegistration;

LineEditElement::LineEditElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
}

void LineEditElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType(ElementString::TextField);

    m_backgroundBrush = std::nullopt;
    if (option) {
        const auto &palette = option->palette;
        if (palette.isBrushSet(palette.currentColorGroup(), QPalette::Base)) {
            m_backgroundBrush = palette.base();
        }
    }

    auto frameOption = qstyleoption_cast<const QStyleOptionFrame *>(option);
    if (const auto lineEdit = qobject_cast<const QLineEdit *>(widget())) {
        m_hasFrame = lineEdit->hasFrame();
    } else if (!widget()) {
        // Widget-less callers indicate the frame through lineWidth, not frameShape.
        m_hasFrame = frameOption && frameOption->lineWidth > 0;
    } else {
        m_hasFrame = frameOption && frameOption->frameShape != QFrame::Shape::NoFrame;
    }

    e->setHints(FrameElement::frameHintsFor(frameOption));
    e->setAttributes(FrameElement::frameAttributesFor(frameOption, widget()));

    e->setHint(u"inside-table"_s, false);
    if (widget() && widget()->parentWidget()) {
        if (auto realParent = widget()->parentWidget()->parentWidget()) {
            if (realParent->inherits("QTableView") || realParent->inherits("QTableWidget")) {
                e->setHint(u"inside-table"_s, true);
            }
        }
    }

    AbstractElement::update(option);
}

void LineEditElement::draw(QPainter *painter, DrawVariant what) const
{
    // For spinboxes and comboboxes, we do not want to draw this element
    // TODO: maybe this should be handleable by the CSS
    const auto parent = widget() ? widget()->parentWidget() : nullptr;
    if (parent && (parent->inherits("QComboBox") || parent->inherits("QAbstractSpinBox"))) {
        return;
    }

    if (std::holds_alternative<QStyle::PrimitiveElement>(what)) {
        switch (std::get<QStyle::PrimitiveElement>(what)) {
        case QStyle::PE_PanelLineEdit:
            drawBackground(painter);
            break;
        case QStyle::PE_FrameLineEdit:
            drawBackground(painter, UnionPainter::Parts::FrameOnly);
            break;
        default:
            break;
        }
    }
}

void LineEditElement::drawBackground(QPainter *painter, UnionPainter::Parts parts) const
{
    if (!m_hasFrame && parts == UnionPainter::Parts::FrameOnly) {
        return;
    }

    UnionPainter unionPainter(painter, properties());
    unionPainter.setParts(m_hasFrame ? parts : UnionPainter::Parts::PanelOnly);

    if (m_backgroundBrush) {
        // TODO Allow using the full brush
        unionPainter.setOverrideBackgroundColor(m_backgroundBrush.value().color());
    }

    unionPainter.setRect(rect());
    unionPainter.draw();
}

QSizeF LineEditElement::contentsSize([[maybe_unused]] const QStyleOption *option, QSizeF contentsSizeFromStyle) const
{
    // LineEdit wants much more simplified size in Widgets, we ignore the width/height set in CSS.
    QMarginsF padding = propertyWrapper().padding();
    return contentsSizeFromStyle.grownBy(padding);
}

// QMarginsF LineEditElement::iconPadding() const
// {
//     if (!m_isValid) {
//         return QMarginsF();
//     }
//     return safePropertyLookup(m_elementProperties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
// }

std::optional<QRectF> LineEditElement::subElementRect(const QStyleOption *option, QStyle::SubElement element) const
{
    if (element == QStyle::SE_LineEditContents) {
        auto r = propertyWrapper().backgroundRectangle(rect());
        return UnionStyle::visualRect(direction(), rect(), r.marginsRemoved(propertyWrapper().padding()));
    }
    return std::nullopt;
}

std::optional<qreal> LineEditElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_DefaultFrameWidth:
        return propertyWrapper().averageBorderSize();
    case QStyle::PM_LineEditIconMargin:
        return propertyWrapper().averagePadding();
    case QStyle::PM_LineEditIconSize:
        return propertyWrapper().iconSize().width();
    case QStyle::PM_TextCursorWidth:
        return style()->QCommonStyle::pixelMetric(pixelMetric, nullptr, widget());
    default:
        break;
    }
    return std::nullopt;
}
