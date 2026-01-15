// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "StyleRule.h"

#include <QHash>

using namespace Union;

class Union::StyleRulePrivate
{
public:
    SelectorList selectors;
    std::unique_ptr<Properties::StyleProperty> properties;
};

StyleRule::StyleRule(std::unique_ptr<StyleRulePrivate> &&d)
    : QObject()
    , d(std::move(d))
{
}

StyleRule::~StyleRule() = default;

SelectorList StyleRule::selectors() const
{
    return d->selectors;
}

void StyleRule::setSelectors(const SelectorList &selectors)
{
    d->selectors = selectors;
}

Properties::StyleProperty *StyleRule::properties() const
{
    return d->properties.get();
}

void StyleRule::setProperties(std::unique_ptr<Properties::StyleProperty> &&newProperties)
{
    d->properties = std::move(newProperties);
}

StyleRule::Ptr StyleRule::create()
{
    return std::make_shared<StyleRule>(std::make_unique<StyleRulePrivate>());
}

QDebug operator<<(QDebug debug, std::shared_ptr<Union::StyleRule> style)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "StyleRule(" << style->selectors() << ")";
    return debug;
}
