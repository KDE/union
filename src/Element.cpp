/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Element.h"

#include <QDebug>
#include <QMetaEnum>
#include <QProperty>
#include <QVariant>

using namespace Union;
using namespace Qt::StringLiterals;

class Union::ElementPrivate
{
public:
    QProperty<QString> type;
    QProperty<QString> id;
    QProperty<Element::States> states;
    QProperty<Element::ColorSet> colorSet;
    QProperty<QStringList> hints;
    QProperty<QVariantMap> attributes;

    std::array<QPropertyNotifier, 6> propertyNotifiers;
};

Element::Element(std::unique_ptr<ElementPrivate> &&dd)
    : QObject(nullptr)
    , d(std::move(dd))
{
    d->propertyNotifiers[0] = d->type.addNotifier([this]() {
        Q_EMIT typeChanged();
    });
    d->propertyNotifiers[1] = d->id.addNotifier([this]() {
        Q_EMIT idChanged();
    });
    d->propertyNotifiers[2] = d->states.addNotifier([this]() {
        Q_EMIT statesChanged();
    });
    d->propertyNotifiers[3] = d->hints.addNotifier([this]() {
        Q_EMIT hintsChanged();
    });
    d->propertyNotifiers[4] = d->attributes.addNotifier([this]() {
        Q_EMIT attributesChanged();
    });
    d->propertyNotifiers[5] = d->colorSet.addNotifier([this]() {
        Q_EMIT colorSetChanged();
    });
}

Element::~Element() = default;

QString Element::type() const
{
    return d->type.value();
}

void Element::setType(const QString &type)
{
    d->type = type;
}

QBindable<QString> Element::bindableType()
{
    return QBindable<QString>(&d->type);
}

QString Element::id() const
{
    return d->id;
}

void Element::setId(const QString &newId)
{
    d->id = newId;
}

QBindable<QString> Element::bindableId()
{
    return QBindable<QString>(&d->id);
}

Element::States Element::states() const
{
    return d->states;
}

void Element::setStates(States newStates)
{
    d->states = newStates;
}

QBindable<Element::States> Union::Element::bindableStates()
{
    return QBindable<States>(&d->states);
}

Element::ColorSet Element::colorSet() const
{
    return d->colorSet;
}

void Element::setColorSet(ColorSet newColorSet)
{
    d->colorSet = newColorSet;
}

QBindable<Element::ColorSet> Element::bindableColorSet()
{
    return QBindable<ColorSet>(&d->colorSet);
}

QStringList Element::hints() const
{
    return d->hints;
}

void Element::setHints(const QStringList &newHints)
{
    d->hints = newHints;
}

QBindable<QStringList> Element::bindableHints()
{
    return QBindable<QStringList>(&d->hints);
}

QVariantMap Element::attributes() const
{
    return d->attributes;
}

void Element::setAttributes(const QVariantMap &attributes)
{
    d->attributes = attributes;
}

QBindable<QVariantMap> Element::bindableAttributes()
{
    return QBindable<QVariantMap>(&d->attributes);
}

bool Element::hasAttribute(const QString &name) const
{
    return (*d->attributes).contains(name);
}

QVariant Element::attribute(const QString &name) const
{
    return (*d->attributes).value(name);
}

Element::Ptr Union::Element::create()
{
    return std::make_shared<Element>(std::make_unique<ElementPrivate>());
}

QDebug operator<<(QDebug debug, Union::Element::Ptr element)
{
    QDebugStateSaver saver(debug);
    debug << "Element(";

    QStringList properties;

    if (!element->type().isEmpty()) {
        properties << u"type: "_s + element->type();
    }

    if (!element->id().isEmpty()) {
        properties << u"id: "_s + element->id();
    }

    if (element->states() != 0) {
        auto flags = QMetaEnum::fromType<Element::States>();
        properties << u"states: "_s + QString::fromUtf8(flags.valueToKeys(element->states()));
    }

    if (!element->hints().isEmpty()) {
        properties << u"hints: "_s + element->hints().join(u", ");
    }

    QString attributes;
    if (!element->attributes().isEmpty()) {
        attributes = QString{};
        for (auto [key, value] : element->attributes().asKeyValueRange()) {
            if (!attributes.isEmpty()) {
                attributes += u", ";
            }
            attributes += key;
            attributes += u": ";
            attributes += value.toString();
        }
        attributes = u"{" + attributes + u"}";
    }

    if (!attributes.isEmpty()) {
        properties << u"attributes: "_s + attributes;
    }

    debug << qPrintable(properties.join(u", "_s)) << ")";
    return debug;
}
