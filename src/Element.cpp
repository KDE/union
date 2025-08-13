// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "Element.h"

#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>
#include <QMetaEnum>
#include <QProperty>
#include <QVariant>

#include "EventHelper.h"

using namespace Union;
using namespace Qt::StringLiterals;

static EventTypeRegistration<ElementChangedEvent> elementRegistration;

class Union::ElementPrivate
{
public:
    QString type;
    QString id;
    Element::States states;
    Element::ColorSet colorSet;
    QSet<QString> hints;
    QVariantMap attributes;
};

Element::Element(std::unique_ptr<ElementPrivate> &&dd)
    : QObject(nullptr)
    , d(std::move(dd))
{
}

Element::~Element() = default;

QString Element::type() const
{
    return d->type;
}

void Element::setType(const QString &type)
{
    if (d->type == type) {
        return;
    }

    d->type = type;

    sendChangeEvent(Change::Type);

    Q_EMIT typeChanged();
    Q_EMIT updated();
}

QString Element::id() const
{
    return d->id;
}

void Element::setId(const QString &newId)
{
    if (d->id == newId) {
        return;
    }

    d->id = newId;

    sendChangeEvent(Change::Id);

    Q_EMIT idChanged();
    Q_EMIT updated();
}

Element::States Element::states() const
{
    return d->states;
}

void Element::setStates(States newStates)
{
    if (d->states == newStates) {
        return;
    }

    d->states = newStates;

    sendChangeEvent(Change::States);

    Q_EMIT statesChanged();
    Q_EMIT updated();
}

Element::ColorSet Element::colorSet() const
{
    return d->colorSet;
}

void Element::setColorSet(ColorSet newColorSet)
{
    if (d->colorSet == newColorSet) {
        return;
    }

    d->colorSet = newColorSet;

    Q_EMIT colorSetChanged();
    Q_EMIT updated();
}

QSet<QString> Element::hints() const
{
    return d->hints;
}

void Element::setHints(const QSet<QString> &newHints)
{
    if (d->hints == newHints) {
        return;
    }

    d->hints = newHints;

    sendChangeEvent(Change::Hints);

    Q_EMIT hintsChanged();
    Q_EMIT updated();
}

void Element::setHint(const QString &name, bool present)
{
    if (present) {
        d->hints.insert(name);
    } else {
        d->hints.remove(name);
    }

    sendChangeEvent(Change::Hints);

    Q_EMIT hintsChanged();
    Q_EMIT updated();
}

bool Union::Element::hasHint(const QString &name)
{
    return d->hints.contains(name);
}

QVariantMap Element::attributes() const
{
    return d->attributes;
}

void Element::setAttributes(const QVariantMap &attributes)
{
    if (d->attributes == attributes) {
        return;
    }

    d->attributes = attributes;

    sendChangeEvent(Change::Attributes);

    Q_EMIT attributesChanged();
    Q_EMIT updated();
}

bool Element::hasAttribute(const QString &name) const
{
    return d->attributes.contains(name);
}

QVariant Element::attribute(const QString &name) const
{
    return d->attributes.value(name);
}

void Element::setAttribute(const QString &name, const QVariant &value)
{
    d->attributes[name] = value;

    sendChangeEvent(Change::Attributes);

    Q_EMIT attributesChanged();
    Q_EMIT updated();
}

QString Element::toString() const
{
    QStringList properties;

    if (!d->type.isEmpty()) {
        properties << u"type: "_s + d->type;
    }

    if (!d->id.isEmpty()) {
        properties << u"id: "_s + d->id;
    }

    if (d->states != 0) {
        auto flags = QMetaEnum::fromType<Element::States>();
        properties << u"states: "_s + QString::fromUtf8(flags.valueToKeys(d->states));
    }

    if (!d->hints.isEmpty()) {
        properties << u"hints: "_s + QStringList(d->hints.begin(), d->hints.end()).join(u", ");
    }

    QString attributes;
    if (!d->attributes.isEmpty()) {
        attributes = QString{};
        for (auto [key, value] : d->attributes.asKeyValueRange()) {
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

    return u"Element("_s + properties.join(u" "_s) + u")"_s;
}

std::size_t Union::Element::cacheKey(std::size_t seed) const
{
    QByteArray serialized;
    serialized.reserve(1000);

    QBuffer buffer(&serialized);
    buffer.open(QIODevice::WriteOnly);

    QDataStream stream(&buffer);
    stream << d->type << d->id << d->states << d->hints << d->attributes;

    return qHash(serialized, seed);
}

Element::Ptr Union::Element::create()
{
    return std::make_shared<Element>(std::make_unique<ElementPrivate>());
}

void Union::Element::sendChangeEvent(Changes changes)
{
    ElementChangedEvent event{changes};
    QCoreApplication::sendEvent(this, &event);
}

QDebug operator<<(QDebug debug, Union::Element::Ptr element)
{
    QDebugStateSaver saver(debug);
    debug << element->toString();
    return debug;
}

ElementChangedEvent::ElementChangedEvent(Element::Changes _changes)
    : QEvent(ElementChangedEvent::s_type)
    , changes(_changes)
{
}
