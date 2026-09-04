// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>

#include <QMetaEnum>

#include <CssParser.h>

#include <Color.h>

namespace CssInput
{
/*
 * Type conversion helper functions.
 *
 * These convert from cssparser types to Union types.
 */

inline float to_px(const cssparser::Dimension &value)
{
    switch (value.unit()) {
    case cssparser::Dimension::Unit::Px:
        return value.value();
    default:
        return 0.0;
    }
}

inline float to_px(const cssparser::Value &value)
{
    if (value.type() != cssparser::Value::Type::Dimension) {
        return 0.0;
    }

    return to_px(value.get<cssparser::Dimension>());
}

inline float to_number(const cssparser::Value &value)
{
    if (value.type() != cssparser::Value::Type::Dimension) {
        return 0.0;
    }

    auto dimension = value.get<cssparser::Dimension>();
    if (dimension.unit() != cssparser::Dimension::Unit::Number) {
        return 0.0;
    }

    return dimension.value();
}

inline Union::Color to_color(const cssparser::Color::Color &color)
{
    switch (color.type()) {
    case cssparser::Color::Color::Type::Empty: {
        return Union::Color{};
    }
    case cssparser::Color::Color::Type::Rgba: {
        auto value = color.get<cssparser::Color::RgbaData>();
        return Union::Color::rgba(value.r(), value.g(), value.b(), value.a());
    }
    case cssparser::Color::Color::Type::Custom: {
        auto value = color.get<cssparser::Color::CustomColorData>();
        QStringList arguments;
        std::ranges::transform(value.arguments(), std::back_inserter(arguments), QString::fromStdString);
        return Union::Color::custom(QString::fromStdString(value.source()), arguments);
    }
    case cssparser::Color::Color::Type::Modified: {
        auto modifiedColor = color.get<cssparser::Color::ModifiedColorData>();

        switch (modifiedColor.operation()) {
        case cssparser::Color::ModifiedColorData::Operation::Unknown:
            return Union::Color{};
        case cssparser::Color::ModifiedColorData::Operation::Add: {
            auto other = modifiedColor.get<std::shared_ptr<cssparser::Color::Color>>();
            return Union::Color::add(to_color(*modifiedColor.color()), to_color(*other));
        }
        case cssparser::Color::ModifiedColorData::Operation::Subtract: {
            auto other = modifiedColor.get<std::shared_ptr<cssparser::Color::Color>>();
            return Union::Color::subtract(to_color(*modifiedColor.color()), to_color(*other));
        }
        case cssparser::Color::ModifiedColorData::Operation::Multiply: {
            auto other = modifiedColor.get<std::shared_ptr<cssparser::Color::Color>>();
            return Union::Color::multiply(to_color(*modifiedColor.color()), to_color(*other));
        }
        case cssparser::Color::ModifiedColorData::Operation::Set: {
            auto data = modifiedColor.get<cssparser::Color::SetOperationData>();
            return Union::Color::set(to_color(*modifiedColor.color()), data.r(), data.g(), data.b(), data.a());
        }
        case cssparser::Color::ModifiedColorData::Operation::Mix: {
            auto data = modifiedColor.get<cssparser::Color::MixOperationData>();
            return Union::Color::mix(to_color(*modifiedColor.color()), to_color(*data.other()), data.amount());
        }
        }
    }
    }

    return Union::Color{};
}

inline Union::Color to_color(const cssparser::Value &value)
{
    if (value.type() != cssparser::Value::Type::Color) {
        return Union::Color{};
    }

    return to_color(value.get<cssparser::Color::Color>());
}

inline std::filesystem::path to_path(const cssparser::Value &value)
{
    if (value.type() != cssparser::Value::Type::Url) {
        return std::filesystem::path{};
    }

    return value.get<std::string>();
}

inline QString to_string(const cssparser::Value &value)
{
    if (value.type() != cssparser::Value::Type::String) {
        return QString{};
    }

    return QString::fromStdString(value.get<std::string>());
}

inline QVariant to_qvariant(const cssparser::Value &value)
{
    switch (value.type()) {
    case cssparser::Value::Type::Empty:
        return QVariant{};
    case cssparser::Value::Type::Dimension:
        return to_px(value);
    case cssparser::Value::Type::String:
        return QString::fromStdString(value.get<std::string>());
    case cssparser::Value::Type::Color:
        return QVariant::fromValue(to_color(value));
    case cssparser::Value::Type::Image:
    case cssparser::Value::Type::Url:
        return QVariant::fromValue(to_path(value));
    case cssparser::Value::Type::Integer:
        return value.get<int>();
    }

    return QVariant{};
}

template<typename T>
inline int toEnumIntValue(/* intentional copy */ std::string value)
{
    const auto metaEnum = QMetaEnum::fromType<T>();

    std::erase(value, '-');

    auto count = metaEnum.keyCount();
    for (int i = 0; i < count; ++i) {
        if (qstrnicmp(metaEnum.key(i), value.c_str(), value.size()) == 0) {
            return metaEnum.value(i);
        }
    }

    return -1;
}

template<typename T>
inline T toEnumValue(const std::string &value)
{
    return static_cast<T>(toEnumIntValue<T>(value));
}

}
