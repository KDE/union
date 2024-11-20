/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "ElementQuery.h"

#include "Theme.h"

#include "union_query_logging.h"

using namespace Union;

class Union::ElementQueryPrivate
{
public:
    std::shared_ptr<Theme> theme;
    QList<Element::Ptr> elements;
    QList<StyleRule::Ptr> styles;
    std::optional<Properties::StyleProperty> properties = std::nullopt;

    inline static const Properties::StyleProperty emptyProperties;
};

ElementQuery::ElementQuery(std::shared_ptr<Theme> theme)
    : d(std::make_unique<ElementQueryPrivate>())
{
    d->theme = theme;
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

QList<StyleRule::Ptr> ElementQuery::matchedRules() const
{
    return d->styles;
}

bool ElementQuery::execute()
{
    qCInfo(UNION_QUERY) << "Trying to match" << d->elements;

    d->styles = d->theme->matches(d->elements);

    if (d->styles.isEmpty()) {
        qCInfo(UNION_QUERY) << "Did not match any style rules!";
        d->properties = std::nullopt;
        return false;
    }

    if (UNION_QUERY().isInfoEnabled()) {
        qCInfo(UNION_QUERY) << "Matched style rules:";
        for (auto entry : std::as_const(d->styles)) {
            qCInfo(UNION_QUERY) << entry;
        }
    }

    d->properties = Properties::StyleProperty();

    for (auto style : std::as_const(d->styles)) {
        Properties::StyleProperty::resolveProperties(style->properties(), d->properties.value());
    }

    return true;
}

bool ElementQuery::hasMatches() const
{
    return d->properties.has_value();
}

const Properties::StyleProperty &ElementQuery::properties() const
{
    return d->properties.has_value() ? d->properties.value() : d->emptyProperties;
}
