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
template<SelectorType type>
void appendFromStream([[maybe_unused]] QDataStream &stream, [[maybe_unused]] Union::SelectorList &destination)
{
    destination.append(Selector::create<type>());
}

template<>
void appendFromStream<SelectorType::Empty>([[maybe_unused]] QDataStream &stream, Union::SelectorList &destination)
{
    destination.append(Selector::create());
}

SelectorPrivateConcept::~SelectorPrivateConcept() = default;

/***** SelectorType::Type *****/
template<>
UNION_EXPORT int SelectorPrivateModel<SelectorType::Type, QString>::weight() const
{
    return 1;
}

template<>
UNION_EXPORT bool SelectorPrivateModel<SelectorType::Type, QString>::matches(Element *element) const
{
    if (data.isEmpty()) {
        return false;
    }

    return QString::compare(element->type(), data, Qt::CaseSensitivity::CaseInsensitive) == 0;
}

template<>
UNION_EXPORT QString SelectorPrivateModel<SelectorType::Type, QString>::toString() const
{
    return u"Type(%1)"_s.arg(data);
}

template<>
void appendFromStream<SelectorType::Type>(QDataStream &stream, Union::SelectorList &destination)
{
    QString type;
    stream >> type;
    destination.append(Selector::create<SelectorType::Type>(type));
}

/***** SelectorType::Id *****/
template<>
UNION_EXPORT int SelectorPrivateModel<SelectorType::Id, QString>::weight() const
{
    return 100;
}

template<>
UNION_EXPORT bool SelectorPrivateModel<SelectorType::Id, QString>::matches(Element *element) const
{
    if (data.isEmpty()) {
        return false;
    }

    return QString::compare(element->id(), data, Qt::CaseSensitivity::CaseInsensitive) == 0;
}

template<>
UNION_EXPORT QString SelectorPrivateModel<SelectorType::Id, QString>::toString() const
{
    return u"Id(%1)"_s.arg(data);
}

template<>
void appendFromStream<SelectorType::Id>(QDataStream &stream, Union::SelectorList &destination)
{
    QString id;
    stream >> id;
    destination.append(Selector::create<SelectorType::Id>(id));
}

/***** SelectorType::State *****/
template<>
UNION_EXPORT int SelectorPrivateModel<SelectorType::State, Element::State>::weight() const
{
    return 10;
}

template<>
UNION_EXPORT bool SelectorPrivateModel<SelectorType::State, Element::State>::matches(Element *element) const
{
    if (data == Element::State::None) {
        return false;
    }

    return element->states() & data;
}

template<>
UNION_EXPORT QString SelectorPrivateModel<SelectorType::State, Element::State>::toString() const
{
    auto e = Element::staticMetaObject.enumerator(Element::staticMetaObject.indexOfEnumerator("State"));
    return u"State(%1)"_s.arg(QString::fromUtf8(e.valueToKeys(int(data))));
}

template<>
void appendFromStream<SelectorType::State>(QDataStream &stream, Union::SelectorList &destination)
{
    Element::State state;
    stream >> state;
    destination.append(Selector::create<SelectorType::State>(state));
}

/***** SelectorType::Hint *****/
template<>
UNION_EXPORT int SelectorPrivateModel<SelectorType::Hint, QString>::weight() const
{
    return 10;
}

template<>
UNION_EXPORT bool SelectorPrivateModel<SelectorType::Hint, QString>::matches(Element *element) const
{
    if (data.isEmpty()) {
        return false;
    }

    return element->hasHint(data);
}

template<>
UNION_EXPORT QString SelectorPrivateModel<SelectorType::Hint, QString>::toString() const
{
    return u"Hint(%1)"_s.arg(data);
}

template<>
void appendFromStream<SelectorType::Hint>(QDataStream &stream, Union::SelectorList &destination)
{
    QString hint;
    stream >> hint;
    destination.append(Selector::create<SelectorType::Hint>(hint));
}

/***** SelectorType::AttributeExists *****/
template<>
UNION_EXPORT int SelectorPrivateModel<SelectorType::AttributeExists, QString>::weight() const
{
    return 10;
}

template<>
UNION_EXPORT bool SelectorPrivateModel<SelectorType::AttributeExists, QString>::matches(Element *element) const
{
    if (data.isEmpty()) {
        return false;
    }

    return element->hasAttribute(data);
}

template<>
UNION_EXPORT QString SelectorPrivateModel<SelectorType::AttributeExists, QString>::toString() const
{
    return u"AttributeExists(%1)"_s.arg(data);
}

template<>
void appendFromStream<SelectorType::AttributeExists>(QDataStream &stream, Union::SelectorList &destination)
{
    QString name;
    stream >> name;
    destination.append(Selector::create<SelectorType::AttributeExists>(name));
}

/***** SelectorType::AttributeEquals *****/
template<>
UNION_EXPORT int SelectorPrivateModel<SelectorType::AttributeEquals, std::pair<QString, QVariant>>::weight() const
{
    return 10;
}

template<>
UNION_EXPORT bool SelectorPrivateModel<SelectorType::AttributeEquals, std::pair<QString, QVariant>>::matches(Element *element) const
{
    if (data.first.isEmpty() || data.second.isNull()) {
        return false;
    }

    return element->attribute(data.first) == data.second;
}

template<>
UNION_EXPORT QString SelectorPrivateModel<SelectorType::AttributeEquals, std::pair<QString, QVariant>>::toString() const
{
    return u"AttributeEquals(key=%1, value=%2)"_s.arg(data.first, data.second.toString());
}

template<>
void appendFromStream<SelectorType::AttributeEquals>(QDataStream &stream, Union::SelectorList &destination)
{
    std::pair<QString, QVariant> data;
    stream >> data;
    destination.append(Selector::create<SelectorType::AttributeEquals>(data));
}

/***** SelectorType::AttributeSubstringMatch *****/
template<>
UNION_EXPORT int SelectorPrivateModel<SelectorType::AttributeSubstringMatch, std::pair<QString, QString>>::weight() const
{
    return 10;
}

template<>
UNION_EXPORT bool SelectorPrivateModel<SelectorType::AttributeSubstringMatch, std::pair<QString, QString>>::matches(Element *element) const
{
    if (data.first.isEmpty() || data.second.isNull()) {
        return false;
    }

    if (element->hasAttribute(data.first)) {
        return element->attribute(data.first).toString().contains(data.second);
    }
    return false;
}

template<>
UNION_EXPORT QString SelectorPrivateModel<SelectorType::AttributeSubstringMatch, std::pair<QString, QString>>::toString() const
{
    return u"AttributeSubstringMatch(key=%1, value=%2)"_s.arg(data.first, data.second);
}

template<>
void appendFromStream<SelectorType::AttributeSubstringMatch>(QDataStream &stream, Union::SelectorList &destination)
{
    std::pair<QString, QString> data;
    stream >> data;
    destination.append(Selector::create<SelectorType::AttributeSubstringMatch>(data));
}

/***** SelectorType::AnyElement *****/
template<>
UNION_EXPORT int SelectorPrivateModel<SelectorType::AnyElement, Empty>::weight() const
{
    return 0;
}

template<>
UNION_EXPORT bool SelectorPrivateModel<SelectorType::AnyElement, Empty>::matches([[maybe_unused]] Element *element) const
{
    return bool(element);
}

template<>
UNION_EXPORT QString SelectorPrivateModel<SelectorType::AnyElement, Empty>::toString() const
{
    return u"AnyElement"_s;
}

/***** SelectorType::ChildCombinator *****/
template<>
UNION_EXPORT int SelectorPrivateModel<SelectorType::ChildCombinator, Empty>::weight() const
{
    return 0;
}

template<>
UNION_EXPORT bool SelectorPrivateModel<SelectorType::ChildCombinator, Empty>::matches([[maybe_unused]] Element *element) const
{
    return false;
}

template<>
UNION_EXPORT QString SelectorPrivateModel<SelectorType::ChildCombinator, Empty>::toString() const
{
    return u"Child"_s;
}

/***** SelectorType::DescendantCombinator *****/
template<>
UNION_EXPORT int SelectorPrivateModel<SelectorType::DescendantCombinator, Empty>::weight() const
{
    return 0;
}

template<>
UNION_EXPORT bool SelectorPrivateModel<SelectorType::DescendantCombinator, Empty>::matches([[maybe_unused]] Element *element) const
{
    return false;
}

template<>
UNION_EXPORT QString SelectorPrivateModel<SelectorType::DescendantCombinator, Empty>::toString() const
{
    return u"Descendant"_s;
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

SelectorType Union::Selector::type() const
{
    if (!d) {
        return SelectorType::Empty;
    }

    return d->type();
}

bool Selector::matches(Element *element) const
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

void Selector::writeToDataStream(QDataStream &stream) const
{
    if (!d) {
        stream << SelectorType::Empty;
        return;
    }

    stream << d->type();
    d->writeToDataStream(stream);
}

bool Selector::isCombinator() const
{
    if (!d) {
        return false;
    }

    return d->isCombinator();
}

Selector::Selector(const std::shared_ptr<const detail::SelectorPrivateConcept> &_d)
    : d(_d)
{
}

bool SelectorList::matches(const QList<Element::Ptr> &elements) const
{
    auto sitr = rbegin();
    auto eitr = elements.rbegin();

    while (sitr != rend()) {
        if (!sitr->matches(eitr->get())) {
            return false;
        }
        sitr++;

        if (sitr != rend() && sitr->isCombinator()) {
            switch (sitr->type()) {
            case SelectorType::ChildCombinator:
                sitr++;
                eitr++;
                break;
            case SelectorType::DescendantCombinator:
                sitr++;
                eitr++;
                while (eitr != elements.rend() && !sitr->matches(eitr->get())) {
                    eitr++;
                }
                break;
            default:
                return false;
            }

            if (eitr == elements.rend()) {
                return false;
            }
        }
    }

    return true;
}

int SelectorList::weight() const
{
    int result = 0;
    for (const auto &selector : (*this)) {
        result += selector.weight();
    }
    return result;
}

QString SelectorList::toString() const
{
    QStringList all;
    std::transform(cbegin(), cend(), std::back_inserter(all), [](auto &selector) {
        return selector.toString();
    });
    return u"SelectorList("_s + all.join(u", ") + u")"_s;
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
    debug << selectors.toString();
    return debug;
}

QDataStream &operator<<(QDataStream &stream, const Union::SelectorList &selectors)
{
    stream << selectors.size();
    for (const auto &selector : selectors) {
        selector.writeToDataStream(stream);
    }

    return stream;
}

template<SelectorType... types>
void appendSelector(SelectorType type, QDataStream &stream, Union::SelectorList &output)
{
    (
        [&] {
            if (type == types) {
                detail::appendFromStream<types>(stream, output);
            }
        }(),
        ... //
    );
}

QDataStream &operator>>(QDataStream &stream, Union::SelectorList &selectors)
{
    qsizetype size;
    stream >> size;

    for (qsizetype i = 0; i < size; ++i) {
        SelectorType selectorType;
        stream >> selectorType;

        appendSelector<SelectorType::Empty,
                       SelectorType::Type,
                       SelectorType::Id,
                       SelectorType::State,
                       SelectorType::Hint,
                       SelectorType::AttributeExists,
                       SelectorType::AttributeEquals,
                       SelectorType::AttributeSubstringMatch,
                       SelectorType::AnyElement,
                       SelectorType::ChildCombinator,
                       SelectorType::DescendantCombinator>(selectorType, stream, selectors);
    }

    return stream;
}
