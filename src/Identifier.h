// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#pragma once

#include "union_export.h"

namespace Union
{

class UNION_EXPORT Identifier
{
public:
    inline Identifier()
    {
    }

    inline Identifier(QAnyStringView view)
        : m_data(view.toString())
    {
    }

    inline Identifier(const QString &string)
        : m_data(string)
    {
    }

    inline Identifier &operator=(QAnyStringView view)
    {
        m_data = view.toString();
        return *this;
    }

    inline bool operator==(const Identifier &other) const
    {
        // Simple case: Empty strings never compare true.
        if (m_data.isEmpty() || other.m_data.isEmpty()) {
            return false;
        }

        // Simple case: If the first characters are different,
        // the identifiers will never match.
        if (!m_data.startsWith(other.m_data.first(1), Qt::CaseInsensitive)) {
            return false;
        }

        if (m_data.compare(other.m_data, Qt::CaseInsensitive) == 0) {
            return true;
        }

        const auto firstStripped = m_data.mid(0).remove(u'-');
        const auto secondStripped = other.m_data.mid(0).remove(u'-');
        return firstStripped.compare(secondStripped, Qt::CaseInsensitive) == 0;
    }

    inline operator QString() const
    {
        return m_data;
    }

    inline bool isEmpty() const
    {
        return m_data.isEmpty();
    }

private:
    QString m_data;
};

}

inline QDataStream &operator<<(QDataStream &stream, const Union::Identifier &identifier)
{
    stream << QString{identifier};
    return stream;
}

inline QDataStream &operator>>(QDataStream &stream, Union::Identifier &rule)
{
    QString data;
    stream >> data;
    rule = Union::Identifier{data};
    return stream;
}
