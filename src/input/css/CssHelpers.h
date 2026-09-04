// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

namespace CssInput
{

/*
 * Common type aliases that slightly reduce the amount of code we need to write.
 */
using CssProperty = cssparser::Property;
using CssValue = cssparser::Value;

/*
 * A helper template that ensures a property group exists. It will forward any
 * property requests to the group. On destruction, if the group has any
 * properties set, the group will be assigned to the right property of the
 * target group.
 */
template<typename Target, typename Getter, typename Setter>
struct PropertyGroupBuilder {
    using PropertyGroup = std::remove_pointer_t<std::invoke_result_t<Getter, Target *>>;

    PropertyGroupBuilder(Target *_target, Getter _getter, Setter _setter)
        : target(_target)
        , getter(_getter)
        , setter(_setter)
    {
        instance = std::invoke(getter, target);
        if (!instance) {
            temporary = std::make_unique<PropertyGroup>();
            instance = temporary.get();
        }
    }

    ~PropertyGroupBuilder()
    {
        if (temporary && temporary->hasAnyValue()) {
            std::invoke(setter, target, std::move(temporary));
        }
    }

    inline PropertyGroup *operator->()
    {
        return instance;
    }

    PropertyGroupBuilder &operator=(const PropertyGroup &group)
    {
        *instance = group;
        return *this;
    }

    std::unique_ptr<PropertyGroup> temporary;
    PropertyGroup *instance;
    Target *target;
    Getter getter;
    Setter setter;
};
}
