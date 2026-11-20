// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "AbstractElement.h"

#include <QApplication>

#include <StyleRegistry.h>

#include "UnionGraphicsEffect.h"

#include "qtwidgets_logging.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;
using namespace Union::Properties;

AbstractElement::AbstractElement(const UnionStyle *style, QWidget *widget)
    : QObject(widget)
    , m_style(style)
    , m_widget(widget)
{
}

AbstractElement::~AbstractElement()
{
}

const UnionStyle *AbstractElement::style() const
{
    return m_style;
}

QWidget *AbstractElement::widget() const
{
    return m_widget;
}

AbstractElement::AbstractElementFlags AbstractElement::flags() const
{
    return m_flags;
}

void AbstractElement::setFlags(AbstractElementFlags newFlags)
{
    m_flags = newFlags;
}

void AbstractElement::setFlag(AbstractElementFlag flag, bool apply)
{
    m_flags.setFlag(flag, apply);
}

bool AbstractElement::isFlagSet(AbstractElementFlag flag) const
{
    return m_flags.testFlag(flag);
}

Union::Element::Ptr AbstractElement::element() const
{
    return m_element;
}

QList<AbstractElement *> AbstractElement::childElements() const
{
    return m_childElements;
}

void AbstractElement::addChildElement(AbstractElement *element)
{
    m_childElements.append(element);
}

void AbstractElement::removeChildElement(AbstractElement *element)
{
    m_childElements.removeAll(element);
}

StyleGroupWrapper AbstractElement::propertyWrapper() const
{
    return m_elementProperties;
}

Union::StylePropertyGroupPtr AbstractElement::properties() const
{
    return m_elementProperties.properties();
}

QRectF AbstractElement::rect() const
{
    return m_rect;
}

Qt::LayoutDirection AbstractElement::direction() const
{
    return m_direction;
}

void AbstractElement::markDirty()
{
    m_changed = true;
}

void AbstractElement::update(const QStyleOption *option)
{
    if (option) {
        if (option->rect != m_rect) {
            m_rect = option->rect;
            layout();
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

        for (const auto &child : std::as_const(m_childElements)) {
            child->update(option);
        }

        layout();
        m_changed = false;
    }
}

void AbstractElement::layout()
{
    m_layout.layout(this);

    for (auto child : std::as_const(m_childElements)) {
        child->m_rect = m_layout.m_layoutMap.value(child).rect;
    }
}

void AbstractElement::draw([[maybe_unused]] QPainter *painter, [[maybe_unused]] DrawVariant what) const
{
    qCWarning(UNION_QTWIDGETS) << "Drawing not implemented for element" << this;
}

QSizeF AbstractElement::contentsSize(QSizeF contentsSizeFromStyle) const
{
    return m_elementProperties.adjustSize(contentsSizeFromStyle, m_flags.testFlag(AbstractElementFlag::ContentsSizeIncludesAveragePadding));
}

QRectF AbstractElement::subElementRect(QStyle::SubElement element) const
{
    Q_UNUSED(element);
    return {};
}

QRectF AbstractElement::subControlRect(QStyle::SubControl subControl) const
{
    Q_UNUSED(subControl);
    return {};
}

qreal AbstractElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    Q_UNUSED(pixelMetric);
    return 0;
}

int AbstractElement::styleHint(QStyle::StyleHint styleHint) const
{
    Q_UNUSED(styleHint);
    return 0;
}

QIcon AbstractElement::standardPixmap(QStyle::StandardPixmap pixmap) const
{
    Q_UNUSED(pixmap);
    return QIcon();
}

bool AbstractElement::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == ElementChangedEvent::s_type) {
        m_changed = true;
    }

    return QObject::eventFilter(target, event);
}

void AbstractElement::queryProperties()
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
        const auto element = widget->property(UnionElementProperty).value<AbstractElement *>();
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

void AbstractElement::setElement(const Union::Element::Ptr &element)
{
    if (element == m_element) {
        return;
    }

    if (m_element) {
        m_element->removeEventFilter(this);
    }

    m_element = element;
    if (m_element) {
        m_element->installEventFilter(this);
    }

    m_changed = true;
}

void AbstractElement::drawBackground(QPainter *painter) const
{
    UnionPainter unionPainter(painter, m_elementProperties.properties());
    unionPainter.setRect(m_rect);
    unionPainter.draw();
}

void AbstractElement::drawFrame(QPainter *painter) const
{
    UnionPainter unionPainter(painter, m_elementProperties.properties());
    unionPainter.setParts(UnionPainter::Parts::FrameOnly);
    unionPainter.setRect(m_rect);
    unionPainter.draw();
}

void AbstractElement::drawPanel(QPainter *painter) const
{
    UnionPainter unionPainter(painter, m_elementProperties.properties());
    unionPainter.setParts(UnionPainter::Parts::PanelOnly);
    unionPainter.setRect(m_rect);
    unionPainter.draw();
}
