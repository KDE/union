/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "QuickElement.h"

#include "Element.h"
#include "Style.h"
#include "Theme.h"

using namespace Union;

Sizes::Sizes()
    : Sizes(0.0, 0.0, 0.0, 0.0)
{
}

Sizes::Sizes(qreal left, qreal right, qreal top, qreal bottom)
    : m_margins(left, top, right, bottom)
{
}

Sizes::Sizes(const QMarginsF &margins)
    : m_margins(margins)
{
}

bool Sizes::operator==(const Sizes &other)
{
    return m_margins == other.m_margins;
}

qreal Sizes::left() const
{
    return m_margins.left();
}

void Sizes::setLeft(qreal newLeft)
{
    m_margins.setLeft(newLeft);
}

qreal Sizes::right() const
{
    return m_margins.right();
}

void Sizes::setRight(qreal newRight)
{
    m_margins.setRight(newRight);
}

qreal Sizes::top() const
{
    return m_margins.top();
}

void Sizes::setTop(qreal newTop)
{
    m_margins.setTop(newTop);
}

qreal Sizes::bottom() const
{
    return m_margins.bottom();
}

void Sizes::setBottom(qreal newBottom)
{
    m_margins.setBottom(newBottom);
}

bool Sizes::isValid() const
{
    return !m_margins.isNull();
}

BordersGroup::BordersGroup(QuickElement *parent)
    : QObject()
    , m_parent(parent)
{
    connect(m_parent, &QuickElement::updated, this, &BordersGroup::onUpdated);
}

Sizes BordersGroup::sizes() const
{
    return m_sizes;
}

QBindable<Sizes> BordersGroup::bindableSizes()
{
    return QBindable<Sizes>(&m_sizes);
}

void BordersGroup::onUpdated()
{
    // Sizes s = m_parent->styleElement()->borderSizes();
    // m_sizes = s;
}

StatesGroup::StatesGroup(QuickElement *parent)
    : QObject()
    , m_parent(parent)
{
    m_activeStates.setBinding([this]() {
        QStringList result;
        if (m_pressed.value()) {
            result << u"pressed"_qs;
        }
        if (m_activeFocus.value()) {
            result << u"activeFocus"_qs;
        }
        if (m_focus.value()) {
            result << u"focus"_qs;
        }
        if (m_hovered.value()) {
            result << u"hovered"_qs;
        }
        if (!m_enabled.value()) {
            result << u"disabled"_qs;
        }
        return result;
    });
}

bool StatesGroup::hovered() const
{
    return m_hovered;
}

void StatesGroup::setHovered(bool newHovered)
{
    m_hovered = newHovered;
}

QBindable<bool> StatesGroup::bindableHovered()
{
    return QBindable<bool>(&m_hovered);
}

bool StatesGroup::focus() const
{
    return m_focus;
}

void StatesGroup::setFocus(bool newFocus)
{
    m_focus = newFocus;
}

QBindable<bool> StatesGroup::bindableFocus()
{
    return QBindable<bool>(&m_focus);
}

bool StatesGroup::activeFocus() const
{
    return m_activeFocus;
}

void StatesGroup::setActiveFocus(bool newActiveFocus)
{
    m_activeFocus = newActiveFocus;
}

QBindable<bool> StatesGroup::bindableActiveFocus()
{
    return QBindable<bool>(&m_activeFocus);
}

bool StatesGroup::pressed() const
{
    return m_pressed;
}

void StatesGroup::setPressed(bool newPressed)
{
    m_pressed = newPressed;
}

QBindable<bool> StatesGroup::bindablePressed()
{
    return QBindable<bool>(&m_pressed);
}

bool StatesGroup::enabled() const
{
    return m_enabled;
}

void StatesGroup::setEnabled(bool newEnabled)
{
    m_enabled = newEnabled;
}

QBindable<bool> StatesGroup::bindableEnabled()
{
    return QBindable<bool>(&m_enabled);
}

QStringList StatesGroup::activeStates() const
{
    return m_activeStates;
}

QBindable<QStringList> StatesGroup::bindableActiveStates()
{
    return QBindable<QStringList>(&m_activeStates);
}

QuickElement::QuickElement(QObject *parent)
    : QQuickAttachedPropertyPropagator(parent)
{
    initialize();

    m_element = Element::create();

    m_bordersGroup = std::make_unique<BordersGroup>(this);
    m_statesGroup = std::make_unique<StatesGroup>(this);

    m_element->bindableStates().setBinding([this]() {
        return m_statesGroup->bindableActiveStates().value();
    });

    m_activeStatesNotifier = m_statesGroup->bindableActiveStates().addNotifier([this]() {
        update();
    });
}

QString QuickElement::type() const
{
    return m_element->type();
}

void QuickElement::setType(const QString &newType)
{
    m_element->setType(newType);
}

QBindable<QString> QuickElement::bindableType()
{
    return m_element->bindableType();
}

QString QuickElement::elementId() const
{
    return m_element->id();
}

void QuickElement::setElementId(const QString &newId)
{
    m_element->setId(newId);
}

QBindable<QString> QuickElement::bindableElementId()
{
    return m_element->bindableId();
}

QStringList QuickElement::hints() const
{
    return m_element->hints();
}

void QuickElement::setHints(const QStringList &newHints)
{
    m_element->setHints(newHints);
}

QBindable<QStringList> QuickElement::bindableHints()
{
    return m_element->bindableHints();
}

QVariantMap QuickElement::attributes() const
{
    return m_element->attributes();
}

void QuickElement::setAttributes(const QVariantMap &newAttributes)
{
    m_element->setAttributes(newAttributes);
}

QBindable<QVariantMap> QuickElement::bindableAttributes()
{
    return m_element->bindableAttributes();
}

Sizes QuickElement::padding() const
{
    return m_padding;
}

QBindable<Sizes> QuickElement::bindablePadding()
{
    return QBindable<Sizes>(&m_padding);
}

Sizes QuickElement::margins() const
{
    return m_margins;
}

QBindable<Sizes> QuickElement::bindableMargins()
{
    return QBindable<Sizes>(&m_margins);
}

StatesGroup *QuickElement::states() const
{
    return m_statesGroup.get();
}

BordersGroup *QuickElement::borders() const
{
    return m_bordersGroup.get();
}

qreal QuickElement::implicitWidth() const
{
    return m_implicitWidth;
}

QBindable<qreal> QuickElement::bindableImplicitWidth()
{
    return QBindable<qreal>(&m_implicitWidth);
}

qreal QuickElement::implicitHeight() const
{
    return m_implicitHeight;
}

QBindable<qreal> QuickElement::bindableImplicitHeight()
{
    return QBindable<qreal>(&m_implicitHeight);
}

Union::ElementQuery QuickElement::buildQuery() const
{
    Union::ElementQuery query(Union::Theme::instance());

    QList<Element::Ptr> elements;
    elements.append(m_element);

    auto parent = qobject_cast<QuickElement *>(attachedParent());
    while (parent) {
        elements.prepend(parent->m_element);
        parent = qobject_cast<QuickElement *>(parent->attachedParent());
    }

    query.setElements(elements);
    return query;
}

QuickElement *QuickElement::qmlAttachedProperties(QObject *parent)
{
    return new QuickElement(parent);
}

void QuickElement::update()
{
    Q_EMIT updated();

    const auto children = attachedChildren();
    for (auto child : children) {
        qobject_cast<QuickElement *>(child)->update();
    }
}
