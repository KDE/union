/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Style.h"

#include "StyleLoader.h"

using namespace Union;

class UNION_NO_EXPORT Style::Private
{
public:
    std::unique_ptr<StyleLoader> styleLoader;
};

Style::Style()
    : QObject(nullptr)
    , d(std::make_unique<Private>())
{

}

Style::~Style() = default;

std::shared_ptr<StyleElement> Style::get(const ElementSelector &selector)
{
//     return d->styleLoader->get(selector);
    return nullptr;
}

std::shared_ptr<Style> Style::instance()
{
    static std::shared_ptr<Style> inst = std::make_shared<Style>();
    return inst;
}

