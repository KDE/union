// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#pragma once

#include "union_export.h"

namespace Union
{

/*!
 * \class Union::Identifier
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief A wrapper around QString that uses an adjusted equality comparison to compare instances.
 *
 * This class wraps a QString with an adjusted equality comparison operator that
 * compares in a case-insensitive manner as well as ignoring any dashes. This
 * allows things like "text-below-icon" to compare equal to "TextBelowIcon".
 *
 * We use this for any identifier used to identify Elements, such as an element's
 * type or ID.
 */
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
        return *this == QStringView(other.m_data);
    }

    inline bool operator==(const QString &other) const
    {
        return *this == QStringView(other);
    }

    inline bool operator==(QStringView other) const
    {
        // Empty strings never compare true, as an empty identifier identifies
        // nothing.
        if (m_data.isEmpty() || other.isEmpty()) {
            return false;
        }

        // If the first characters are different, the identifiers will never
        // match and there is no need to go into the more complex comparison
        // below.
        if (!m_data.startsWith(other.first(1), Qt::CaseInsensitive)) {
            return false;
        }

        // If the strings compare equal already there is no need to copy and
        // do the manipulation below.
        if (m_data.compare(other, Qt::CaseInsensitive) == 0) {
            return true;
        }

        // We want strings like "some-thing" to compare equal to "SomeThing", so
        // strip out all dashes and then compare if we did not hit any of the
        // earlier cases already.
        const auto firstStripped = QString{m_data}.remove(u'-');
        const auto secondStripped = QString{other}.remove(u'-');
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

using IdentifierList = QList<Identifier>;
using IdentifierVariantHash = QHash<Identifier, QVariant>;

}

// Overload of qHash that ensures the same semantics for operator== also apply
// to hashes.
inline size_t qHash(const Union::Identifier &identifier, size_t seed = 0)
{
    if (identifier.isEmpty()) {
        // Ensure empty identifiers have a unique hash, as they should never
        // compare equal.
        return quintptr(&identifier);
    }

    auto string = QString{identifier}.toLower().remove(u'-');
    return qHash(string, seed);
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
