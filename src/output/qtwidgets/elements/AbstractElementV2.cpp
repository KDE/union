// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "AbstractElementV2.h"

// #include <QApplication>
// #include <QDebug>
// #include <QPainter>
// #include <QStyle>
#include <StyleRegistry.h>

// #include "SharedNames.h"
// #include "UnionGraphicsEffect.h"
#include "UnionPainter.h"
// #include "UnionStyle.h"

#include "TextSubElement.h"

#include "qtwidgets_logging.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;
using namespace Union::Properties;

AbstractElementV2::AbstractElementV2(UnionStyle *style, QWidget *widget)
    : QObject(widget)
    , m_style(style)
    , m_widget(widget)
{
}

AbstractElementV2::~AbstractElementV2()
{
}

AbstractElementV2::AbstractElementFlags AbstractElementV2::flags() const
{
    return m_flags;
}

void AbstractElementV2::setFlags(AbstractElementFlags newFlags)
{
    m_flags = newFlags;
}

void AbstractElementV2::setFlag(AbstractElementFlag flag, bool apply)
{
    m_flags.setFlag(flag, apply);
}

Union::Element::Ptr AbstractElementV2::element() const
{
    return m_element;
}

TextSubElement *Union::Widgets::AbstractElementV2::textSubElement() const
{
    if (!m_textSubElement) {
        m_textSubElement = new TextSubElement(m_style, m_widget);
    }

    return m_textSubElement;
}

void AbstractElementV2::update(const QStyleOption *option)
{
    if (option) {
        if (option->rect != m_rect) {
            m_rect = option->rect;
            // layout();
        }

        if (m_element) {
            Union::Element::States states;
            auto state = option->state;
            states.setFlag(Element::State::Hovered, state.testFlag(QStyle::State_MouseOver));
            states.setFlag(Element::State::ActiveFocus, state.testFlag(QStyle::State_HasFocus));
            states.setFlag(Element::State::VisualFocus, state.testFlag(QStyle::State_KeyboardFocusChange) && state.testFlag(QStyle::State_HasFocus));
            states.setFlag(Element::State::Pressed, state.testFlag(QStyle::State_Sunken));
            states.setFlag(Element::State::Checked, state.testFlag(QStyle::State_On));
            states.setFlag(Element::State::Disabled, !state.testFlag(QStyle::State_Enabled));

            if (state.testFlag(QStyle::State_Raised)) {
                states.setFlag(Element::State::Pressed, false);
            }

            if (state.testFlag(QStyle::State_Off)) {
                states.setFlag(Union::Element::State::Checked, false);
            }

            m_element->setStates(states);
        }
    }

    if (m_changed) {
        queryProperties();
        // layout();
        m_changed = false;
    }
}

bool AbstractElementV2::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == ElementChangedEvent::s_type) {
        m_changed = true;
    }

    return QObject::eventFilter(target, event);
}

void AbstractElementV2::queryProperties()
{
    if (!m_changed) {
        return;
    }

    ElementQuery query(StyleRegistry::instance()->defaultStyle());

    Union::ElementList elements;
    QWidget *widget = nullptr;

    if (m_element) {
        elements.append(m_element);
        widget = m_widget ? m_widget->parentWidget() : nullptr;
    } else {
        widget = m_widget;
    }

    while (widget) {
        const auto element = widget->property(UnionElementProperty).value<AbstractElementV2 *>();
        if (element) {
            elements.prepend(element->element());
        }
        widget = widget->parentWidget();
    }

    if (elements.isEmpty()) {
        return;
    }

    query.setElements(elements);
    query.execute();

    m_elementProperties = query.properties();
}

void AbstractElementV2::createElement(const Union::Identifier &type)
{
    m_element = Element::create();
    m_element->setType(type);
    m_element->installEventFilter(this);
    m_changed = true;
}

QSizeF AbstractElementV2::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    return contentsSizeFromStyle;
    // return applyPaddingToSize(contentsSizeFromStyle);
}

QRectF AbstractElementV2::subElementRect(QStyle::SubElement element) const
{
    Q_UNUSED(element);
    return {};
}

QRectF AbstractElementV2::subControlRect(QStyle::SubControl subControl) const
{
    Q_UNUSED(subControl);
    return {};
}

qreal AbstractElementV2::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    Q_UNUSED(pixelMetric);
    return 0;
}

int AbstractElementV2::styleHint(QStyle::StyleHint styleHint) const
{
    Q_UNUSED(styleHint);
    return 0;
}

QIcon AbstractElementV2::standardPixmap(QStyle::StandardPixmap pixmap) const
{
    Q_UNUSED(pixmap);
    return QIcon();
}

void AbstractElementV2::draw([[maybe_unused]] QPainter *painter, [[maybe_unused]] DrawVariant what) const
{
    qCWarning(UNION_QTWIDGETS) << "Drawing not implemented for element" << this;
}

void AbstractElementV2::drawBackground(QPainter *painter) const
{
    if (m_elementProperties) {
        UnionPainter unionPainter(painter, m_elementProperties);
        unionPainter.setRect(m_rect);
        unionPainter.draw();
    }
}

void AbstractElementV2::drawFrame(QPainter *painter) const
{
    if (m_elementProperties) {
        UnionPainter unionPainter(painter, m_elementProperties);
        unionPainter.setParts(UnionPainter::Parts::FrameOnly);
        unionPainter.setRect(m_rect);
        unionPainter.draw();
    }
}

void AbstractElementV2::drawPanel(QPainter *painter) const
{
    if (m_elementProperties) {
        UnionPainter unionPainter(painter, m_elementProperties);
        unionPainter.setParts(UnionPainter::Parts::PanelOnly);
        unionPainter.setRect(m_rect);
        unionPainter.draw();
    }
}
