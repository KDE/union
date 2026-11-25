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

const UnionStyle *AbstractElement::style() const
{
    return m_style;
}

QWidget *AbstractElement::widget() const
{
    return m_widget;
}

bool AbstractElement::isValid() const
{
    return m_elementProperties.properties() != nullptr;
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

Union::Element::Ptr AbstractElement::ensureElement()
{
    if (!m_element) {
        setElement(Union::Element::create());
    }
    return m_element;
}

QList<AbstractElement *> AbstractElement::childElements() const
{
    return m_childElements;
}

void AbstractElement::addChildElement(AbstractElement *element)
{
    if (element) {
        m_childElements.append(element);
        m_changed = true;
    }
}

void AbstractElement::removeChildElement(AbstractElement *element)
{
    if (element) {
        m_childElements.removeAll(element);
        m_changed = true;
    }
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

void AbstractElement::setRect(const QRectF &newRect)
{
    if (newRect == m_rect) {
        return;
    }

    m_rect = newRect;

    if (!m_flags.testFlag(AbstractElementFlag::SubElement)) {
        layout();
    }
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
    if (option && !m_flags.testFlag(AbstractElementFlag::SubElement)) {
        setRect(option->rect);

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
            child->markDirty();
            child->update(option);
        }

        layout();

        if (m_widget) {
            auto effect = qobject_cast<UnionGraphicsEffect *>(m_widget->graphicsEffect());
            if (effect) {
                effect->setDrawRect(m_rect);
                effect->setProperties(m_elementProperties.properties());
            }
        }

        m_changed = false;
    }
}

void AbstractElement::layout()
{
    if (m_flags.testFlag(AbstractElementFlag::SubElement)) {
        return;
    }

    m_layout.layout(this);

    for (auto child : std::as_const(m_childElements)) {
        child->setRect(m_layout.m_layoutMap.value(child).rect);
    }
}

void AbstractElement::draw([[maybe_unused]] QPainter *painter, [[maybe_unused]] DrawVariant what) const
{
    qCWarning(UNION_QTWIDGETS) << "Drawing not implemented for element" << this;
}

QSizeF AbstractElement::contentsSize([[maybe_unused]] const QStyleOption *option, QSizeF contentsSizeFromStyle) const
{
    return m_elementProperties.adjustSize(contentsSizeFromStyle, m_flags.testFlag(AbstractElementFlag::ContentsSizeIncludesAveragePadding));
}

QRectF AbstractElement::subElementRect([[maybe_unused]] const QStyleOption *option, [[maybe_unused]] QStyle::SubElement element) const
{
    return style()->QCommonStyle::subElementRect(element, option, widget());
}

QRectF AbstractElement::subControlRect([[maybe_unused]] const QStyleOption *option, [[maybe_unused]] QStyle::SubControl subControl) const
{
    return {};
}

qreal AbstractElement::pixelMetric([[maybe_unused]] const QStyleOption *option, [[maybe_unused]] QStyle::PixelMetric pixelMetric) const
{
    return style()->QCommonStyle::pixelMetric(pixelMetric, option, widget());
}

int AbstractElement::styleHint([[maybe_unused]] const QStyleOption *option, [[maybe_unused]] QStyle::StyleHint styleHint) const
{
    return style()->QCommonStyle::styleHint(styleHint, option, widget());
}

QIcon AbstractElement::standardPixmap([[maybe_unused]] const QStyleOption *option, [[maybe_unused]] QStyle::StandardPixmap pixmap) const
{
    return style()->standardPixmap(pixmap, option, widget());
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
    }

    if (m_widget) {
        if (!m_flags.testFlag(AbstractElementFlag::SubElement)) {
            widget = m_widget->parentWidget();
        } else {
            widget = m_widget;
        }
    }

    while (widget) {
        const auto element = widget->property(UnionElementProperty).value<AbstractElement *>();
        if (element && element->element()) {
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

void AbstractElement::drawBackground(QPainter *painter, UnionPainter::Parts parts) const
{

    UnionPainter unionPainter(painter, m_elementProperties.properties());
    unionPainter.setParts(parts);
    unionPainter.setRect(m_rect);
    unionPainter.draw();
}

QRectF AbstractElement::visualRect(AbstractElement *childElement) const
{
    if (!childElement) {
        return {};
    }

    return UnionStyle::visualRect(m_direction, m_rect, childElement->rect());
}
