// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "Selector.h"

#include <ranges>

#include <QMetaEnum>

#include "Element.h"

#include "union_export.h"

using namespace Union;
using namespace Qt::StringLiterals;

namespace Union::detail
{
SelectorPrivate::~SelectorPrivate() = default;

template<>
int SelectorPrivateImpl<SelectorType::Type, QString>::weight() const
{
    return 1;
}

template<>
bool SelectorPrivateImpl<SelectorType::Type, QString>::matches(std::shared_ptr<Element> element) const
{
    if (data.isEmpty()) {
        return false;
    }

    return element->type() == data;
}

template<>
QString SelectorPrivateImpl<SelectorType::Type, QString>::toString() const
{
    return u"Type(%1)"_s.arg(data);
}

template<>
int detail::SelectorPrivateImpl<SelectorType::Id, QString>::weight() const
{
    return 100;
}

template<>
bool detail::SelectorPrivateImpl<SelectorType::Id, QString>::matches(std::shared_ptr<Element> element) const
{
    if (data.isEmpty()) {
        return false;
    }

    return element->id() == data;
}

template<>
QString detail::SelectorPrivateImpl<SelectorType::Id, QString>::toString() const
{
    return u"Id(%1)"_s.arg(data);
}

template<>
int detail::SelectorPrivateImpl<SelectorType::State, Element::State>::weight() const
{
    return 10;
}

template<>
bool detail::SelectorPrivateImpl<SelectorType::State, Element::State>::matches(std::shared_ptr<Element> element) const
{
    if (data == Element::State::None) {
        return false;
    }

    return element->states() & data;
}

template<>
QString detail::SelectorPrivateImpl<SelectorType::State, Element::State>::toString() const
{
    auto e = Element::staticMetaObject.enumerator(Element::staticMetaObject.indexOfEnumerator("State"));
    return u"State(%1)"_s.arg(QString::fromUtf8(e.valueToKeys(int(data))));
}

template<>
int detail::SelectorPrivateImpl<SelectorType::ColorSet, Element::ColorSet>::weight() const
{
    return 10;
}

template<>
bool detail::SelectorPrivateImpl<SelectorType::ColorSet, Element::ColorSet>::matches(std::shared_ptr<Element> element) const
{
    if (data == Element::ColorSet::None) {
        return false;
    }

    return element->colorSet() == data;
}

template<>
QString detail::SelectorPrivateImpl<SelectorType::ColorSet, Element::ColorSet>::toString() const
{
    auto e = Element::staticMetaObject.enumerator(Element::staticMetaObject.indexOfEnumerator("ColorSet"));
    return u"ColorSet(%1)"_s.arg(QString::fromUtf8(e.valueToKeys(int(data))));
}

template<>
int detail::SelectorPrivateImpl<SelectorType::Hint, QString>::weight() const
{
    return 10;
}

template<>
bool detail::SelectorPrivateImpl<SelectorType::Hint, QString>::matches(std::shared_ptr<Element> element) const
{
    if (data.isEmpty()) {
        return false;
    }
    return element->hints().contains(data);
}

template<>
QString SelectorPrivateImpl<SelectorType::Hint, QString>::toString() const
{
    return u"Hint(%1)"_s.arg(data);
}

template<>
int SelectorPrivateImpl<SelectorType::Attribute, std::pair<QString, QVariant>>::weight() const
{
    return 10;
}

template<>
bool SelectorPrivateImpl<SelectorType::Attribute, std::pair<QString, QVariant>>::matches(std::shared_ptr<Element> element) const
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
QString SelectorPrivateImpl<SelectorType::Attribute, std::pair<QString, QVariant>>::toString() const
{
    return u"Attribute(key=%1, value=%2)"_s.arg(data.first, data.second.toString());
}

template<>
int SelectorPrivateImpl<SelectorType::AnyOf, SelectorList>::weight() const
{
    auto weights = std::views::transform(data, [](auto selector) {
        return selector.weight();
    });
    return *std::ranges::max_element(weights);
}

template<>
bool SelectorPrivateImpl<SelectorType::AnyOf, SelectorList>::matches(std::shared_ptr<Element> element) const
{
    return std::any_of(data.cbegin(), data.cend(), [element](auto &selector) {
        return selector.matches(element);
    });
}

template<>
QString SelectorPrivateImpl<SelectorType::AnyOf, SelectorList>::toString() const
{
    QStringList all;
    std::transform(data.cbegin(), data.cend(), std::back_inserter(all), [](auto &selector) {
        return selector.toString();
    });
    return u"AnyOf(%1)"_s.arg(all.join(u","));
}

template<>
int SelectorPrivateImpl<SelectorType::AllOf, SelectorList>::weight() const
{
    auto weights = std::views::transform(data, [](auto selector) {
        return selector.weight();
    });
    return std::accumulate(weights.begin(), weights.end(), 0);
}

template<>
bool SelectorPrivateImpl<SelectorType::AllOf, SelectorList>::matches(std::shared_ptr<Element> element) const
{
    return std::all_of(data.cbegin(), data.cend(), [element](auto &selector) {
        return selector.matches(element);
    });
}

template<>
QString SelectorPrivateImpl<SelectorType::AllOf, SelectorList>::toString() const
{
    QStringList all;
    std::transform(data.cbegin(), data.cend(), std::back_inserter(all), [](auto &selector) {
        return selector.toString();
    });
    return u"AllOf(%1)"_s.arg(all.join(u","));
}

}

bool Selector::isValid() const
{
    return bool(d);
}

int Union::Selector::weight() const
{
    if (!d) {
        return 0;
    }

    return d->weight();
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
        return u"Selector(Invalid)"_s;
    }

    return d->toString();
}

Selector::Selector(std::shared_ptr<const detail::SelectorPrivate> _d)
    : d(_d)
{
}

bool SelectorList::matches(const QList<Element::Ptr> &elements) const
{
    auto sitr = begin();
    auto eitr = elements.begin();

    while (sitr != end() && eitr != elements.end()) {
        if (sitr->matches(*eitr)) {
            sitr++;
        }
        eitr++;
    }

    return sitr == end() && eitr == elements.end();
}

int SelectorList::weight() const
{
    int result = 0;
    for (const auto &selector : (*this)) {
        result += selector.weight();
    }
    return result;
}

void SelectorList::appendAnyOf(const SelectorList &selectors)
{
    if (selectors.size() == 1) {
        append(selectors.first());
    } else {
        append(Selector::create<SelectorType::AnyOf>(selectors));
    }
}

void SelectorList::appendAllOf(const SelectorList &selectors)
{
    if (selectors.size() == 1) {
        append(selectors.first());
    } else {
        append(Selector::create<SelectorType::AllOf>(selectors));
    }
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
