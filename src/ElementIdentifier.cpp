/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "ElementIdentifier.h"

#include <QDebug>

using namespace Union;

ElementIdentifier::ElementIdentifier(const QString &style, const QString &element, const QString &subElement)
    : m_style(style)
    , m_element(element)
    , m_subElement(subElement)
{
}

ElementIdentifier::ElementIdentifier(const ElementIdentifier &other)
{
    *this = other;
}

ElementIdentifier::ElementIdentifier(ElementIdentifier &&other)
{
    *this = other;
}

ElementIdentifier::~ElementIdentifier() = default;

ElementIdentifier &ElementIdentifier::operator=(const ElementIdentifier &other)
{
    m_style = other.m_style;
    m_element = other.m_element;
    m_subElement = other.m_subElement;
    return *this;
}

ElementIdentifier &ElementIdentifier::operator=(ElementIdentifier &&other)
{
    m_style = std::move(other.m_style);
    m_element = std::move(other.m_element);
    m_subElement = std::move(other.m_subElement);
    return *this;
}

bool ElementIdentifier::operator==(const ElementIdentifier &other) const
{
    return m_style == other.m_style && m_element == other.m_element && m_subElement == other.m_subElement;
}

QString ElementIdentifier::style() const
{
    return m_style;
}

void ElementIdentifier::setStyle(const QString &name)
{
    m_style = name;
}

QString ElementIdentifier::element() const
{
    return m_element;
}

void ElementIdentifier::setElement(const QString &name)
{
    m_element = name;
}

QString ElementIdentifier::subElement() const
{
    return m_subElement;
}

void ElementIdentifier::setSubElement(const QString &name)
{
    m_subElement = name;
}
