/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>

#include <QDebug>
#include <QObject>

#include "union_export.h"

namespace Union
{

class UNION_EXPORT ElementIdentifier
{
public:
    // ElementIdentifier();
    ElementIdentifier(const QString &style = QString{}, const QString &element = QString{}, const QString &subElement = QString{});
    ElementIdentifier(const ElementIdentifier &other);
    ElementIdentifier(ElementIdentifier &&other);

    ~ElementIdentifier();

    ElementIdentifier &operator=(const ElementIdentifier &other);
    ElementIdentifier &operator=(ElementIdentifier &&other);

    bool operator==(const ElementIdentifier &other) const;

    QString style() const;
    void setStyle(const QString &name);

    QString element() const;
    void setElement(const QString &name);

    QString subElement() const;
    void setSubElement(const QString &name);

private:
    QString m_style;
    QString m_element;
    QString m_subElement;
};

}

template<>
struct std::hash<Union::ElementIdentifier> {
    std::size_t operator()(const Union::ElementIdentifier &identifier) const
    {
        QStringList items = {identifier.style(), identifier.element(), identifier.subElement()};
        return qHash(items);
    }
};

UNION_EXPORT QDebug operator<<(QDebug debug, const Union::ElementIdentifier &identifier);
