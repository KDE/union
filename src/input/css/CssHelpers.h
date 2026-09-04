// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <type_traits>
#include <variant>

#include <QStringList>

#include <CssParser.h>

namespace CssInput
{

/*
 * Common type aliases that slightly reduce the amount of code we need to write.
 */
using CssProperty = cssparser::Property;
using CssValue = cssparser::Value;

/*
 * Helper templates to construct a switch-case like syntax switching on strings
 * rather than integers. This is mostly syntactic sugar to make it simpler to
 * write functions that effectively do
 * "if (string == "something") do_a() else if (string == "something_else) do_b()"
 *
 * It consists of the switchString() function below which expects a switch value
 * and a list of cases.
 */

/*
 * A helper tag struct to indicate a Case should check if it starts with a
 * string rather than match the full string.
 */
struct StartsWith {
    StartsWith(QAnyStringView _string)
        : string(_string.toString())
    {
    }

    QString string;
};

/*
 * A helper tag struct to indicate a Case should check if it ends with a
 * string rather than match the full string.
 */
struct EndsWith {
    EndsWith(QAnyStringView _string)
        : string(_string.toString())
    {
    }

    QString string;
};

/*
 * A single case branch for switchString. This can take either a string, a list
 * of strings or one of the above tagged strings, along with a lambda to execute
 * if the string should match. Matching is done on a case insensitive basis.
 */
template<typename T>
struct Case {
    Case(QAnyStringView _string, T &&_block)
        : values(_string.toString())
        , block(_block)
    {
    }

    Case(std::initializer_list<QAnyStringView> &&_list, T &&_block)
        : block(_block)
    {
        QStringList stringValues;
        std::ranges::transform(_list, std::back_inserter(stringValues), &QAnyStringView::toString);
        values = stringValues;
    }

    Case(StartsWith &&_startsWith, T &&_block)
        : values(_startsWith)
        , block(_block)
    {
    }

    Case(EndsWith &&_endsWith, T &&_block)
        : values(_endsWith)
        , block(_block)
    {
    }

    std::variant<QString, QStringList, StartsWith, EndsWith> values;

    T block;

    bool compare(const QString &input)
    {
        if (std::holds_alternative<QString>(values)) {
            return input.compare(std::get<QString>(values), Qt::CaseInsensitive) == 0;
        } else if (std::holds_alternative<QStringList>(values)) {
            return std::get<QStringList>(values).contains(input, Qt::CaseInsensitive);
        } else if (std::holds_alternative<StartsWith>(values)) {
            return input.startsWith(std::get<StartsWith>(values).string, Qt::CaseInsensitive);
        } else if (std::holds_alternative<EndsWith>(values)) {
            return input.endsWith(std::get<EndsWith>(values).string, Qt::CaseInsensitive);
        }
        return false;
    }
};

/*
 * A default case that gets executed if none of the other cases matched. This takes a single
 * lambda containing the code to execute.
 */
template<typename T>
struct Default {
    T block;
};

/*
 * A switch-like statement construct that switches based on a string value rather than an
 * integer. It can be passed either a cssparser::Property which switches on the property name,
 * a cssparser::Value which switches on the string it contains or a normal string.
 *
 * Example usage:
 *
 * ```
 * switchString(property
 *     ,Default{[&]() {
 *         // Default case behaviour.
 *     }}
 *     ,Case{"some-value", [&] {
 *         // Do whatever "some-value" requires.
 *     }}
 *     ,Case{"some-other-value", [&](auto &&value) {
 *         // Do whatever "some-other-value" requires. `value` is the value of the property.
 *     }}
 * );
 * ```
 */
template<typename Input, typename DefaultFunction, typename... Functions>
void switchString(const Input &input, Default<DefaultFunction> defaultCase, Case<Functions>... functions)
{
    bool found = false;
    (
        [&] {
            if (found) {
                return;
            }

            QString string;
            if constexpr (std::is_same_v<Input, cssparser::Property>) {
                string = QString::fromStdString(input.name());
            } else if constexpr (std::is_same_v<Input, cssparser::Value>) {
                string = QString::fromStdString(input.template get<std::string>());
            } else if constexpr (std::is_same_v<Input, std::string>) {
                string = QString::fromStdString(input);
            } else {
                string = input;
            }

            if (string.isEmpty()) {
                return;
            }

            if (functions.compare(string)) {
                found = true;

                if constexpr (std::is_invocable_v<Functions, cssparser::Value>) {
                    if constexpr (std::is_same_v<Input, cssparser::Property>) {
                        functions.block(std::forward<cssparser::Value>(input.value()));
                    } else {
                        functions.block(std::forward<const Input>(input));
                    }
                } else {
                    functions.block();
                }
            }
        }(),
        ... //
    );

    if (!found) {
        if constexpr (std::is_invocable_v<DefaultFunction, cssparser::Value>) {
            if constexpr (std::is_same_v<Input, cssparser::Property>) {
                defaultCase.block(std::forward<cssparser::Value>(input.value()));
            } else {
                defaultCase.block(std::forward<const Input>(input));
            }
        } else {
            defaultCase.block();
        }
    }
}

/*
 * An overload of switchString() that does not require a default case.
 */
template<typename Input, typename... Functions>
void switchString(const Input &input, Case<Functions>... functions)
{
    switchString(input, Default{[]() { }}, functions...);
}

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
