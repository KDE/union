/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Element.h"

#include <QDebug>
#include <QProperty>

using namespace Union;

class Union::ElementPrivate
{
public:
    QProperty<QString> type;
    QProperty<QString> id;
    QProperty<QStringList> states;
    QProperty<QStringList> hints;
    QProperty<QVariantMap> attributes;

    std::array<QPropertyNotifier, 5> propertyNotifiers;
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

QBindable<QString> Union::Element::bindableId()
{
    return QBindable<QString>(&d->id);
}

QStringList Element::states() const
{
    return d->states;
}

void Element::setStates(const QStringList &newStates)
{
    d->states = newStates;
}

QBindable<QStringList> Union::Element::bindableStates()
{
    return QBindable<QStringList>(&d->states);
}

QStringList Element::hints() const
{
    return d->hints;
}

void Element::setHints(const QStringList &newHints)
{
    d->hints = newHints;
}

QBindable<QStringList> Union::Element::bindableHints()
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

QBindable<QVariantMap> Union::Element::bindableAttributes()
{
    return QBindable<QVariantMap>(&d->attributes);
}

bool Union::Element::hasAttribute(const QString &name) const
{
    return (*d->attributes).contains(name);
}

QVariant Union::Element::attribute(const QString &name) const
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
    static const auto emptyString = u"(empty)"_qs;
    const auto type = element->type().isEmpty() ? emptyString : element->type();
    const auto id = element->id().isEmpty() ? emptyString : element->id();
    const auto states = element->states().isEmpty() ? emptyString : element->states().join(u", ");
    const auto hints = element->hints().isEmpty() ? emptyString : element->hints().join(u", ");
    debug << "type:" << qPrintable(type);
    debug << "id:" << qPrintable(id);
    debug << "states:" << qPrintable(states);
    debug << "hints:" << qPrintable(hints);
    debug << ")";
    return debug;
}
