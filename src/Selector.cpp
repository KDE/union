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
SelectorPrivateConcept::~SelectorPrivateConcept() = default;

template<>
int SelectorPrivateModel<SelectorType::Type, QString>::weight() const
{
    return 1;
}

template<>
bool SelectorPrivateModel<SelectorType::Type, QString>::matches(std::shared_ptr<Element> element) const
{
    if (data.isEmpty()) {
        return false;
    }

    return element->type() == data;
}

template<>
QString SelectorPrivateModel<SelectorType::Type, QString>::toString() const
{
    return u"Type(%1)"_s.arg(data);
}

template<>
int SelectorPrivateModel<SelectorType::Id, QString>::weight() const
{
    return 100;
}

template<>
bool SelectorPrivateModel<SelectorType::Id, QString>::matches(std::shared_ptr<Element> element) const
{
    if (data.isEmpty()) {
        return false;
    }

    return element->id() == data;
}

template<>
QString SelectorPrivateModel<SelectorType::Id, QString>::toString() const
{
    return u"Id(%1)"_s.arg(data);
}

template<>
int SelectorPrivateModel<SelectorType::State, Element::State>::weight() const
{
    return 10;
}

template<>
bool SelectorPrivateModel<SelectorType::State, Element::State>::matches(std::shared_ptr<Element> element) const
{
    if (data == Element::State::None) {
        return false;
    }

    return element->states() & data;
}

template<>
QString SelectorPrivateModel<SelectorType::State, Element::State>::toString() const
{
    auto e = Element::staticMetaObject.enumerator(Element::staticMetaObject.indexOfEnumerator("State"));
    return u"State(%1)"_s.arg(QString::fromUtf8(e.valueToKeys(int(data))));
}

template<>
int SelectorPrivateModel<SelectorType::ColorSet, Element::ColorSet>::weight() const
{
    return 10;
}

template<>
bool SelectorPrivateModel<SelectorType::ColorSet, Element::ColorSet>::matches(std::shared_ptr<Element> element) const
{
    if (data == Element::ColorSet::None) {
        return false;
    }

    return element->colorSet() == data;
}

template<>
QString SelectorPrivateModel<SelectorType::ColorSet, Element::ColorSet>::toString() const
{
    auto e = Element::staticMetaObject.enumerator(Element::staticMetaObject.indexOfEnumerator("ColorSet"));
    return u"ColorSet(%1)"_s.arg(QString::fromUtf8(e.valueToKeys(int(data))));
}

template<>
int SelectorPrivateModel<SelectorType::Hint, QString>::weight() const
{
    return 10;
}

template<>
bool SelectorPrivateModel<SelectorType::Hint, QString>::matches(std::shared_ptr<Element> element) const
{
    if (data.isEmpty()) {
        return false;
    }
    return element->hints().contains(data);
}

template<>
QString SelectorPrivateModel<SelectorType::Hint, QString>::toString() const
{
    return u"Hint(%1)"_s.arg(data);
}

template<>
int SelectorPrivateModel<SelectorType::Attribute, std::pair<QString, QVariant>>::weight() const
{
    return 10;
}

template<>
bool SelectorPrivateModel<SelectorType::Attribute, std::pair<QString, QVariant>>::matches(std::shared_ptr<Element> element) const
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
QString SelectorPrivateModel<SelectorType::Attribute, std::pair<QString, QVariant>>::toString() const
{
    return u"Attribute(key=%1, value=%2)"_s.arg(data.first, data.second.toString());
}

template<>
int SelectorPrivateModel<SelectorType::AnyOf, SelectorList>::weight() const
{
    auto weights = std::views::transform(data, [](auto selector) {
        return selector.weight();
    });
    return *std::ranges::max_element(weights);
}

template<>
bool SelectorPrivateModel<SelectorType::AnyOf, SelectorList>::matches(std::shared_ptr<Element> element) const
{
    return std::any_of(data.cbegin(), data.cend(), [element](auto &selector) {
        return selector.matches(element);
    });
}

template<>
QString SelectorPrivateModel<SelectorType::AnyOf, SelectorList>::toString() const
{
    QStringList all;
    std::transform(data.cbegin(), data.cend(), std::back_inserter(all), [](auto &selector) {
        return selector.toString();
    });
    return u"AnyOf(%1)"_s.arg(all.join(u","));
}

template<>
int SelectorPrivateModel<SelectorType::AllOf, SelectorList>::weight() const
{
    auto weights = std::views::transform(data, [](auto selector) {
        return selector.weight();
    });
    return std::accumulate(weights.begin(), weights.end(), 0);
}

template<>
bool SelectorPrivateModel<SelectorType::AllOf, SelectorList>::matches(std::shared_ptr<Element> element) const
{
    return std::all_of(data.cbegin(), data.cend(), [element](auto &selector) {
        return selector.matches(element);
    });
}

template<>
QString SelectorPrivateModel<SelectorType::AllOf, SelectorList>::toString() const
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

Selector::Selector(std::shared_ptr<const detail::SelectorPrivateConcept> _d)
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
