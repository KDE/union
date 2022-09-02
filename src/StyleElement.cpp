/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "StyleElement.h"

using namespace Union;

class UNION_NO_EXPORT StyleElement::Private
{
public:

};

StyleElement::StyleElement(QObject *parent)
    : QObject(parent)
{
}

StyleElement::~StyleElement() = default;
