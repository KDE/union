/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>
#include <optional>

#include <QObject>
#include <QVariant>

#include "Selector.h"
#include "properties/StyleProperty.h"

#include "union_export.h"

namespace Union
{

class StyleRulePrivate;

/**
 * A set of style properties that should be applied to a certain set of elements.
 *
 * This class defines a set of properties to apply to an element, along with a
 * list of selectors that should match for this style to apply.
 */
class UNION_EXPORT StyleRule : public QObject, public std::enable_shared_from_this<StyleRule>
{
    Q_OBJECT

public:
    using Ptr = std::shared_ptr<StyleRule>;

    StyleRule(std::unique_ptr<StyleRulePrivate> &&d);
    ~StyleRule() override;

    SelectorList selectors() const;
    void setSelectors(const SelectorList &selectors);

    const Properties::StyleProperty &properties() const;
    void setProperties(const Properties::StyleProperty &newProperties);

    static Ptr create();

private:
    const std::unique_ptr<StyleRulePrivate> d;
};

}

UNION_EXPORT QDebug operator<<(QDebug debug, std::shared_ptr<Union::StyleRule> style);
