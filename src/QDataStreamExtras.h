// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>

#include <QDataStream>

template<typename T>
QDataStream &operator<<(QDataStream &stream, const std::optional<T> &value)
{
    stream << value.has_value();
    if (value.has_value()) {
        stream << value.value();
    }
    return stream;
}

template<typename T>
QDataStream &operator>>(QDataStream &stream, std::optional<T> &value)
{
    bool hasValue;
    stream >> hasValue;
    if (hasValue) {
        T data;
        stream >> data;
        value = data;
    } else {
        value = std::nullopt;
    }
    return stream;
}

inline QDataStream &operator<<(QDataStream &stream, const std::filesystem::path &path)
{
    stream << QByteArray(path.string());
    return stream;
}

inline QDataStream &operator>>(QDataStream &stream, std::filesystem::path &path)
{
    QByteArray data;
    stream >> data;
    path = std::filesystem::path(data.toStdString());
    return stream;
}

inline QDataStream &operator<<(QDataStream &stream, const std::filesystem::file_time_type &timeStamp)
{
    stream << qint64(timeStamp.time_since_epoch().count());
    return stream;
}

inline QDataStream &operator>>(QDataStream &stream, std::filesystem::file_time_type &timeStamp)
{
    qint64 duration;
    stream >> duration;

    timeStamp = std::filesystem::file_time_type(std::filesystem::file_time_type::duration(duration));

    return stream;
}
