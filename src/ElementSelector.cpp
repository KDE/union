/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "ElementSelector.h"

#include <QDebug>

using namespace Union;

class UNION_NO_EXPORT ElementSelector::Private
{
public:
    QString elementName;
};

ElementSelector::ElementSelector()
    : d(std::make_unique<Private>())
{
}

ElementSelector::ElementSelector(const ElementSelector& other)
    : d(std::make_unique<Private>())
{
    *this = other;
}

ElementSelector::ElementSelector(ElementSelector && other)
{
    *this = other;
}

ElementSelector::~ElementSelector() = default;

ElementSelector &ElementSelector::operator=(const ElementSelector &other)
{
    d->elementName = other.d->elementName;
    return *this;
}

ElementSelector &ElementSelector::operator=(ElementSelector &&other)
{
    d = std::move(other.d);
    return *this;
}

QString ElementSelector::elementName() const
{
    return d->elementName;
}

void ElementSelector::setElementName(const QString& name)
{
    d->elementName = name;
}
