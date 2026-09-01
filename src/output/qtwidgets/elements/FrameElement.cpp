// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "FrameElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QLineEdit>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

FrameElement::FrameElement(const QStyleOptionFrame *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_frameOption(option)
{
    update();
}

FrameElement::~FrameElement()
{
}

void FrameElement::update()
{
    updateSubElementList();
    layout();
}

void FrameElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.ControlElement) {
    case QStyle::CE_FocusFrame:
    case QStyle::CE_ShapedFrame:
        drawBackground(painter);
        break;
    }
    // We should be prepared to draw any potential frame element
    // even if not explicitly casted to do so.
    switch (enums.PrimitiveElement) {
    case QStyle::PE_Frame:
    case QStyle::PE_FrameDefaultButton:
    case QStyle::PE_FrameDockWidget:
    case QStyle::PE_FrameFocusRect:
    case QStyle::PE_FrameGroupBox:
    case QStyle::PE_FrameLineEdit:
    case QStyle::PE_FrameMenu:
    case QStyle::PE_FrameStatusBarItem:
    case QStyle::PE_FrameTabWidget:
    case QStyle::PE_FrameWindow:
    case QStyle::PE_FrameButtonBevel:
    case QStyle::PE_FrameButtonTool:
    case QStyle::PE_FrameTabBarBase:
        drawFrame(painter);
        break;
    }
}

void FrameElement::drawFrame(QPainter *painter) const
{
    // Follow the expectations: If frame has panel shape,
    // we need to draw the background too. In HLine/VLine cases
    // we can just skip it
    switch (m_frameOption->frameShape) {
    case QFrame::NoFrame:
        // Draw nothing
        return;
    case QFrame::Box:
    case QFrame::Panel:
    case QFrame::WinPanel:
    case QFrame::StyledPanel:
        drawBackground(painter);
        break;
    case QFrame::HLine:
    case QFrame::VLine:
        AbstractElement::drawFrame(painter);
        break;
    }
}

void FrameElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::Frame);
}

QVariantMap FrameElement::elementAttributes() const
{
    QVariantMap map;
    switch (m_frameOption->frameShape) {
    case QFrame::NoFrame:
        map[u"shape"_s] = u"no-frame"_s;
        break;
    case QFrame::Box:
        map[u"shape"_s] = u"box"_s;
        break;
    case QFrame::Panel:
    case QFrame::WinPanel:
    case QFrame::StyledPanel:
        map[u"shape"_s] = u"panel"_s;
        break;
    case QFrame::HLine:
        map[u"shape"_s] = u"horizontal-line"_s;
        break;
    case QFrame::VLine:
        map[u"shape"_s] = u"vertical-line"_s;
        break;
    default:
        break;
    }

    return map;
}

QStringList FrameElement::elementHints() const
{
    return frameHints(m_frameOption);
}

qreal FrameElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_DefaultFrameWidth:
        return averageBorderSize();
    default:
        break;
    }
    return 0;
}
