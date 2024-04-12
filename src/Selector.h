/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>

#include <QList>
#include <QString>
#include <QVariant>

#include "union_export.h"

namespace Union
{

class Element;

struct SelectorPrivate;

class UNION_EXPORT Selector
{
public:
    enum class SelectorType {
        Empty,
        Type,
        Id,
        State,
        Hint,
        Attribute,
        AnyOf,
        AllOf,
    };

    Selector();
    Selector(SelectorType type, const QVariant &data);
    Selector(const Selector &other);
    Selector(Selector &&other);
    ~Selector();

    Selector &operator=(const Selector &other);
    Selector &operator=(Selector &&other);

    bool operator==(const Selector &other) const;

    SelectorType type() const;
    QVariant data() const;

    bool isValid() const;

    bool matches(std::shared_ptr<Element> element) const;
    QString toString() const;

private:
    std::unique_ptr<SelectorPrivate> d;
};

using SelectorList = QList<Selector>;
UNION_EXPORT bool selectorListMatches(const SelectorList &selectors, const QList<std::shared_ptr<Element>> &elements);

}

UNION_EXPORT QDebug operator<<(QDebug debug, const Union::Selector &selector);
UNION_EXPORT QDebug operator<<(QDebug debug, const Union::SelectorList &selectors);
