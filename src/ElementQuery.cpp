// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "ElementQuery.h"

#include "LruCache.h"
#include "Style.h"

#include "union_query_logging.h"

using namespace Union;

class Union::ElementQueryPrivate
{
public:
    std::shared_ptr<Style> style;
    QList<Element::Ptr> elements;
    QList<StyleRule::Ptr> styles;
    std::shared_ptr<Properties::StyleProperty> properties = nullptr;

    inline static LruCache<std::size_t, std::shared_ptr<Properties::StyleProperty>, 500> s_matchesCache;
};

ElementQuery::ElementQuery(std::shared_ptr<Style> style)
    : d(std::make_unique<ElementQueryPrivate>())
{
    d->style = style;
}

ElementQuery::~ElementQuery() noexcept = default;

QList<Element::Ptr> ElementQuery::elements() const
{
    return d->elements;
}

void ElementQuery::setElements(const QList<Element::Ptr> &elements)
{
    d->elements = elements;
}

bool ElementQuery::execute()
{
    qCInfo(UNION_QUERY) << "Trying to match" << d->elements;

    auto cacheKey = elementListCacheKey(d->elements, QHashSeed::globalSeed());
    if (auto cached = ElementQueryPrivate::s_matchesCache.value(cacheKey); cached) {
        qCInfo(UNION_QUERY) << "Matched from cache";
        d->properties = cached.value();
        return true;
    }

    d->styles = d->style->matches(d->elements);

    if (d->styles.isEmpty()) {
        qCInfo(UNION_QUERY) << "Did not match any style rules!";
        d->properties = nullptr;
        ElementQueryPrivate::s_matchesCache.insert(cacheKey, nullptr);
        return false;
    }

    if (UNION_QUERY().isInfoEnabled()) {
        qCInfo(UNION_QUERY) << "Matched style rules:";
        for (auto entry : std::as_const(d->styles)) {
            qCInfo(UNION_QUERY) << entry;
        }
    }

    d->properties = std::make_shared<Properties::StyleProperty>();

    for (auto style : std::as_const(d->styles)) {
        Properties::StyleProperty::resolveProperties(style->properties(), d->properties.get());
    }

    ElementQueryPrivate::s_matchesCache.insert(cacheKey, d->properties);

    return true;
}

bool ElementQuery::hasMatches() const
{
    return bool(d->properties);
}

Properties::StyleProperty *ElementQuery::properties() const
{
    return d->properties.get();
}
