/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Style.h"

#include <QUrl>

#include "ElementIdentifier.h"
#include "StyleLoader.h"
#include "input/plasmasvg/PlasmaSvgLoader.h"

#include "union_logging.h"

using namespace Union;

class UNION_NO_EXPORT Style::Private
{
public:
    std::unique_ptr<StyleLoader> styleLoader;

    std::unordered_map<ElementIdentifier, std::shared_ptr<StyleElement>> elements;
};

Style::Style()
    : QObject(nullptr)
    , d(std::make_unique<Private>())
{
}

Style::~Style() = default;

std::shared_ptr<StyleElement> Style::get(const ElementIdentifier &selector)
{
    if (!d->styleLoader) {
        d->styleLoader = std::make_unique<PlasmaSvgLoader>(instance());
        if (!d->styleLoader->load()) {
            qCCritical(UNION_GENERAL) << "Failed to load input!";
            return nullptr;
        }
    }

    auto element = d->elements.find(selector);
    if (element != d->elements.end()) {
        return element->second;
    }

    return nullptr;
}

void Style::insert(const ElementIdentifier &identifier, std::shared_ptr<StyleElement> element)
{
    auto pair = std::make_pair(identifier, element);
    d->elements.insert(pair);
}

std::shared_ptr<Style> Style::instance()
{
    static std::shared_ptr<Style> inst = std::make_shared<Style>();
    return inst;
}

