// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "WidgetElement.h"

#include <QFormLayout>

using namespace Qt::StringLiterals;
using namespace Union::Widgets;

WidgetElement::WidgetElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
{
    update(option);
}

void WidgetElement::update(const QStyleOption *option)
{
    AbstractElement::update(option);
    layout();
}

void WidgetElement::draw(QPainter *painter, DrawVariant what) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_Widget:
        // Custom KDE widget. It uses PE_Widget for drawing,
        // but we want to redirect it to TabBar instead.
        if (widget() && widget()->inherits("KMultiTabBar")) {
            drawKMultiTabBar(painter);
        } else {
            drawBackground(painter);
        }

        break;
    }
}

int WidgetElement::styleHint(const QStyleOption *option, QStyle::StyleHint styleHint) const
{
    // TODO: We need to figure a way to expose these in CSS in a nice manner
    switch (styleHint) {
    case QStyle::SH_FormLayoutFieldGrowthPolicy:
        return QFormLayout::ExpandingFieldsGrow;
    case QStyle::SH_FormLayoutWrapPolicy:
        return QFormLayout::DontWrapRows;
    case QStyle::SH_FormLayoutFormAlignment:
        return Qt::AlignLeft | Qt::AlignTop;
    case QStyle::SH_FormLayoutLabelAlignment:
        return Qt::AlignRight;
    default:
        break;
    }
    return 0;
}

void WidgetElement::drawKMultiTabBar(QPainter *painter) const
{
    if (!widget()) {
        return;
    }
    QStyleOptionTabBarBase opt;
    opt.initFrom(widget());
    enum class Position {
        Left,
        Right,
        Top,
        Bottom,
    };
    const Position position = static_cast<Position>(widget()->property("position").toInt());
    switch (position) {
    case Position::Left:
        opt.shape = QTabBar::RoundedWest;
        break;
    case Position::Right:
        opt.shape = QTabBar::RoundedEast;
        break;
    case Position::Top:
        opt.shape = QTabBar::RoundedNorth;
        break;
    case Position::Bottom:
        opt.shape = QTabBar::RoundedSouth;
        break;
    }
    style()->drawPrimitive(QStyle::PE_FrameTabBarBase, &opt, painter, widget());
}
