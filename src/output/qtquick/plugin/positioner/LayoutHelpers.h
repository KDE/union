// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QQuickItem>

#include <PropertiesTypes.h>

#include "LayoutItem.h"

namespace Union
{
namespace Quick
{

template<typename T>
concept convertible_to_qreal = requires(T t) { std::is_convertible<T, qreal>(); };

template<convertible_to_qreal... Args>
inline qreal spacedSize(qreal spacing, Args &&...sizes)
{
    constexpr auto count = sizeof...(sizes);
    qreal result = (qreal(sizes) + ...);

    if constexpr (count > 1) {
        qreal totalSpacing = ((sizes > 0 ? spacing : 0.0) + ...);
        result += totalSpacing > 0 ? totalSpacing - spacing : 0.0;
    }

    return result;
}

}
}
