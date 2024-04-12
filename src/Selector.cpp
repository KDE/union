/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Selector.h"

#include "Element.h"

using namespace Union;
using namespace Qt::StringLiterals;

struct Union::SelectorPrivate {
    SelectorPrivate(Selector::SelectorType t)
        : type(t)
    {
    }

    virtual ~SelectorPrivate()
    {
    }

    virtual bool matches(std::shared_ptr<Element> element) = 0;
    virtual QString toString() = 0;

    const Selector::SelectorType type;
    QVariant data;
};

struct TypeSelectorPrivate : public SelectorPrivate {
    TypeSelectorPrivate()
        : SelectorPrivate(Selector::SelectorType::Type)
    {
    }

    bool matches(std::shared_ptr<Element> element) override
    {
        return element->type() == data.toString();
    }

    QString toString() override
    {
        return u"Type(%1)"_qs.arg(data.toString());
    }
};

struct IdSelectorPrivate : public SelectorPrivate {
    IdSelectorPrivate()
        : SelectorPrivate(Selector::SelectorType::Id)
    {
    }

    bool matches(std::shared_ptr<Element> element) override
    {
        return element->id() == data.toString();
    }

    QString toString() override
    {
        return u"Id(%1)"_qs.arg(data.toString());
    }
};

struct StateSelectorPrivate : public SelectorPrivate {
    StateSelectorPrivate()
        : SelectorPrivate(Selector::SelectorType::State)
    {
    }

    bool matches(std::shared_ptr<Element> element) override
    {
        return element->states().contains(data.toString());
    }

    QString toString() override
    {
        return u"State(%1)"_qs.arg(data.toString());
    }
};

struct HintSelectorPrivate : public SelectorPrivate {
    HintSelectorPrivate()
        : SelectorPrivate(Selector::SelectorType::Hint)
    {
    }

    bool matches(std::shared_ptr<Element> element) override
    {
        return element->hints().contains(data.toString());
    }

    QString toString() override
    {
        return u"Hint(%1)"_qs.arg(data.toString());
    }
};

struct AttributeSelectorPrivate : public SelectorPrivate {
    AttributeSelectorPrivate()
        : SelectorPrivate(Selector::SelectorType::Attribute)
    {
    }

    bool matches(std::shared_ptr<Element> element) override
    {
        auto key = data.toMap().value(u"key"_qs).toString();
        auto value = data.toMap().value(u"value"_qs);
        if (element->hasAttribute(key)) {
            return element->attribute(key) == value;
        }
        return false;
    }

    QString toString() override
    {
        return u"Attribute(key=%1, value=%2)"_qs.arg(data.toMap().value(u"key"_qs).toString(), data.toMap().value(u"value"_qs).toString());
    }
};

struct AllOfSelectorPrivate : public SelectorPrivate {
    AllOfSelectorPrivate()
        : SelectorPrivate(Selector::SelectorType::AllOf)
    {
    }

    bool matches(std::shared_ptr<Element> element) override
    {
        const auto selectors = data.value<SelectorList>();
        return std::all_of(selectors.cbegin(), selectors.cend(), [element](auto &selector) {
            return selector.matches(element);
        });
    }

    QString toString() override
    {
        QStringList all;
        const auto selectors = data.value<SelectorList>();
        std::transform(selectors.begin(), selectors.end(), std::back_inserter(all), [](auto &selector) {
            return selector.toString();
        });
        return u"AllOf(%1)"_qs.arg(all.join(u","));
    }
};

struct AnyOfSelectorPrivate : public SelectorPrivate {
    AnyOfSelectorPrivate()
        : SelectorPrivate(Selector::SelectorType::AnyOf)
    {
    }

    bool matches(std::shared_ptr<Element> element) override
    {
        const auto selectors = data.value<SelectorList>();
        return std::any_of(selectors.begin(), selectors.end(), [element](auto &selector) {
            return selector.matches(element);
        });
    }

    QString toString() override
    {
        QStringList all;
        const auto selectors = data.value<SelectorList>();
        std::transform(selectors.begin(), selectors.end(), std::back_inserter(all), [](auto &selector) {
            return selector.toString();
        });
        return u"AnyOf(%1)"_qs.arg(all.join(u","));
    }
};

std::unique_ptr<SelectorPrivate> make_private(Selector::SelectorType type, const QVariant &data)
{
    std::unique_ptr<SelectorPrivate> d;
    switch (type) {
    case Selector::SelectorType::Empty:
        d = nullptr;
        break;
    case Selector::SelectorType::Type:
        d = std::make_unique<TypeSelectorPrivate>();
        break;
    case Selector::SelectorType::Id:
        d = std::make_unique<IdSelectorPrivate>();
        break;
    case Selector::SelectorType::State:
        d = std::make_unique<StateSelectorPrivate>();
        break;
    case Selector::SelectorType::Hint:
        d = std::make_unique<HintSelectorPrivate>();
        break;
    case Selector::SelectorType::Attribute:
        d = std::make_unique<AttributeSelectorPrivate>();
        break;
    case Selector::SelectorType::AllOf:
        d = std::make_unique<AllOfSelectorPrivate>();
        break;
    case Selector::SelectorType::AnyOf:
        d = std::make_unique<AnyOfSelectorPrivate>();
        break;
    }

    d->data = data;
    return d;
}

Selector::Selector()
    : d(nullptr)
{
}

Selector::Selector(SelectorType type, const QVariant &data)
    : d(make_private(type, data))
{
}

Selector::Selector(const Selector &other)
{
    *this = other;
}

Selector::Selector(Selector &&other)
{
    *this = std::move(other);
}

Selector::~Selector() noexcept
{
}

Selector &Selector::operator=(const Selector &other)
{
    if (other.d) {
        d = make_private(other.d->type, other.d->data);
    } else {
        d = nullptr;
    }
    return *this;
}

Selector &Selector::operator=(Selector &&other)
{
    d = std::move(other.d);
    return *this;
}

bool Selector::operator==(const Selector &other) const
{
    if (d == other.d) {
        return true;
    }

    return d && other.d && d->type == other.d->type && d->data == other.d->data;
}

Selector::SelectorType Selector::type() const
{
    if (d) {
        return d->type;
    }

    return SelectorType::Empty;
}

QVariant Selector::data() const
{
    if (d) {
        return d->data;
    }

    return QVariant{};
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

bool selectorListMatches(const SelectorList &selectors, const QList<Element::Ptr> &elements)
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
