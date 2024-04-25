/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Selector.h"

#include <QMetaEnum>

#include "Element.h"

using namespace Union;
using namespace Qt::StringLiterals;

Selector::SelectorPrivate::~SelectorPrivate() = default;

template<>
bool Selector::SelectorPrivateImpl<SelectorType::Type, QString>::matches(std::shared_ptr<Element> element) const
{
    if (data.isEmpty()) {
        return false;
    }

    return element->type() == data;
}

template<>
QString Selector::SelectorPrivateImpl<SelectorType::Type, QString>::toString() const
{
    return u"Type(%1)"_s.arg(data);
}

template<>
bool Selector::SelectorPrivateImpl<SelectorType::Id, QString>::matches(std::shared_ptr<Element> element) const
{
    if (data.isEmpty()) {
        return false;
    }

    return element->id() == data;
}

template<>
QString Selector::SelectorPrivateImpl<SelectorType::Id, QString>::toString() const
{
    return u"Id(%1)"_qs.arg(data);
}

template<>
bool Selector::SelectorPrivateImpl<SelectorType::State, Element::State>::matches(std::shared_ptr<Element> element) const
{
    if (data == Element::State::None) {
        return false;
    }

    return element->states() & data;
}

template<>
QString Selector::SelectorPrivateImpl<SelectorType::State, Element::State>::toString() const
{
    auto e = Element::staticMetaObject.enumerator(Element::staticMetaObject.indexOfEnumerator("State"));
    return u"State(%1)"_qs.arg(QString::fromUtf8(e.valueToKeys(int(data))));
}

template<>
bool Selector::SelectorPrivateImpl<SelectorType::Hint, QString>::matches(std::shared_ptr<Element> element) const
{
    if (data.isEmpty()) {
        return false;
    }
    return element->hints().contains(data);
}

template<>
QString Selector::SelectorPrivateImpl<SelectorType::Hint, QString>::toString() const
{
    return u"Hint(%1)"_qs.arg(data);
}

template<>
bool Selector::SelectorPrivateImpl<SelectorType::Attribute, std::pair<QString, QVariant>>::matches(std::shared_ptr<Element> element) const
{
    if (data.first.isEmpty() || data.second.isNull()) {
        return false;
    }

    if (element->hasAttribute(data.first)) {
        return element->attribute(data.first) == data.second;
    }
    return false;
}

template<>
QString Selector::SelectorPrivateImpl<SelectorType::Attribute, std::pair<QString, QVariant>>::toString() const
{
    return u"Attribute(key=%1, value=%2)"_qs.arg(data.first, data.second.toString());
}

template<>
bool Selector::SelectorPrivateImpl<SelectorType::AnyOf, SelectorList>::matches(std::shared_ptr<Element> element) const
{
    return std::any_of(data.cbegin(), data.cend(), [element](auto &selector) {
        return selector.matches(element);
    });
}

template<>
QString Selector::SelectorPrivateImpl<SelectorType::AnyOf, SelectorList>::toString() const
{
    QStringList all;
    std::transform(data.cbegin(), data.cend(), std::back_inserter(all), [](auto &selector) {
        return selector.toString();
    });
    return u"AnyOf(%1)"_qs.arg(all.join(u","));
}

template<>
bool Selector::SelectorPrivateImpl<SelectorType::AllOf, SelectorList>::matches(std::shared_ptr<Element> element) const
{
    return std::all_of(data.cbegin(), data.cend(), [element](auto &selector) {
        return selector.matches(element);
    });
}

template<>
QString Selector::SelectorPrivateImpl<SelectorType::AllOf, SelectorList>::toString() const
{
    QStringList all;
    std::transform(data.cbegin(), data.cend(), std::back_inserter(all), [](auto &selector) {
        return selector.toString();
    });
    return u"AllOf(%1)"_qs.arg(all.join(u","));
}

bool Selector::isValid() const
{
    return bool(d);
}

bool Selector::matches(std::shared_ptr<Element> element) const
{
    if (!d) {
        return false;
    }

    return d->matches(element);
}

QString Selector::toString() const
{
    if (!d) {
        return u"Selector(Invalid)"_qs;
    }

    return d->toString();
}

Selector::Selector(std::shared_ptr<const SelectorPrivate> _d)
    : d(_d)
{
}

bool Union::selectorListMatches(const SelectorList &selectors, const QList<Element::Ptr> &elements)
{
    auto sitr = selectors.begin();
    auto eitr = elements.begin();

    while (sitr != selectors.end() && eitr != elements.end()) {
        if (sitr->matches(*eitr)) {
            sitr++;
        } else {
            eitr++;
        }
    }

    return sitr == selectors.end() && eitr == (elements.end() - 1);
}

QDebug operator<<(QDebug debug, const Union::Selector &selector)
{
    QDebugStateSaver saver(debug);
    debug << selector.toString();
    return debug;
}

QDebug operator<<(QDebug debug, const Union::SelectorList &selectors)
{
    QDebugStateSaver saver(debug);

    QStringList all;
    std::transform(selectors.cbegin(), selectors.cend(), std::back_inserter(all), [](auto &selector) {
        return selector.toString();
    });
    debug.nospace() << "SelectorList(" << all.join(u" ") << ")";
    return debug;
}
