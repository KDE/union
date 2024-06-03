/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "QuickStyle.h"

#include <QQmlEngine>

#include <Element.h>
#include <StyleRule.h>
#include <Theme.h>

#include "QuickElement.h"

using namespace Union;

QuickStyle::QuickStyle(QObject *parent)
    : QQuickAttachedPropertyPropagator(parent)
{
    m_properties = std::make_unique<StylePropertyGroup>();

    initialize();
}

StylePropertyGroup *QuickStyle::properties() const
{
    return m_properties.get();
}

Union::ElementQuery *QuickStyle::query() const
{
    if (m_element) {
        return m_element->query();
    }

    return nullptr;
}

QuickStyle *QuickStyle::qmlAttachedProperties(QObject *parent)
{
    return new QuickStyle(parent);
}

void QuickStyle::attachedParentChange(QQuickAttachedPropertyPropagator *, QQuickAttachedPropertyPropagator *)
{
    update();
}

void QuickStyle::setElement(QuickElement *newElement)
{
    if (newElement == m_element) {
        return;
    }

    if (m_element) {
        m_element->disconnect(this);
    }

    m_element = newElement;

    if (m_element) {
        connect(m_element, &QuickElement::updated, this, &QuickStyle::update);
    }
}

void QuickStyle::update()
{
    auto element = qobject_cast<QuickElement *>(qmlAttachedPropertiesObject<QuickElement>(parent(), false));
    auto parentStyle = attachedParent();

    while (!element && parentStyle) {
        element = qobject_cast<QuickElement *>(qmlAttachedPropertiesObject<QuickElement>(parentStyle->parent(), false));
        parentStyle = parentStyle->attachedParent();
    }

    setElement(element);

    if (!element) {
        return;
    }

    auto query = element->query();

    if (!query || !query->hasMatches()) {
        return;
    }

    m_properties->update(query->properties());

    Q_EMIT updated();

    const auto children = attachedChildren();
    for (auto child : children) {
        qobject_cast<QuickStyle *>(child)->update();
    }
}
