// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>

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
    QString type;
    QString id;
    Element::States states;
    Element::ColorSet colorSet;
    QStringList hints;
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

QStringList Element::hints() const
{
    return d->hints;
}

void Element::setHints(const QStringList &newHints)
{
    if (d->hints == newHints) {
        return;
    }

    d->hints = newHints;
    Q_EMIT hintsChanged();
    Q_EMIT updated();
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
