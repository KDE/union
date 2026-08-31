// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include "elements/AbstractElement.h"
#include <QStyleOption>

#include <LruCache.h>

class AbstractElement;
class UnionStyle;

namespace ElementCache
{
namespace detail
{
template<typename ElementType, typename OptionType>
struct TypeHelper {
    using elementType = ElementType;
    using optionType = OptionType;
};

template<typename EnumType, int enumValue>
struct EnumToType {
};

static Union::LruCache<size_t, std::shared_ptr<AbstractElement>> s_elementCache;

template<typename EnumType, int enumValue>
void element(std::shared_ptr<AbstractElement> &output, size_t hash, const UnionStyle *style, const QStyleOption *option, const QWidget *widget)
{
    using ElementType = EnumToType<EnumType, enumValue>::elementType;
    using OptionType = EnumToType<EnumType, enumValue>::optionType;

    if (detail::s_elementCache.contains(hash)) {
        output = detail::s_elementCache.value(hash).value();
        output->update();
        return;
    }

    if (const auto cast = qstyleoption_cast<const OptionType *>(option)) {
        output = std::make_shared<ElementType>(cast, style, widget);
        detail::s_elementCache.insert(hash, output);
    }
    return;
}
}

template<typename EnumType, int... choices>
static std::shared_ptr<AbstractElement> element(int enumValue, size_t hash, const UnionStyle *style, const QStyleOption *option, const QWidget *widget)
{
    std::shared_ptr<AbstractElement> result;
    (
        [&] {
            if (enumValue == choices) {
                detail::element<EnumType, choices>(result, hash, style, option, widget);
            }
        }(),
        ... //
    );

    return result;
}

inline static void clear()
{
    detail::s_elementCache.clear();
}
}
