/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "LoadingContext.h"

#include "RYMLHelpers.h"

using namespace Union;

ContextCleanup::ContextCleanup(ContextData &_data)
    : data(_data)
{
}

ContextCleanup::~ContextCleanup() noexcept
{
    if (flags & CleanupFlag::Selector && !data.selectors.isEmpty()) {
        data.selectors.pop();
    }

    if (flags & CleanupFlag::Path && !data.paths.isEmpty()) {
        data.paths.pop();
    }

    if (flags & CleanupFlag::Prefix && !data.prefixes.isEmpty()) {
        data.prefixes.pop();
    }

    if (flags & CleanupFlag::ElementName && !data.elementNames.isEmpty()) {
        data.elementNames.pop();
    }

    if (flags & CleanupFlag::ColorSet && !data.colorSets.isEmpty()) {
        data.colorSets.pop();
    }
}

LoadingContext::LoadingContext(Theme::Ptr _theme)
    : theme(_theme)
{
}

ContextCleanup LoadingContext::pushFromNode(ryml::ConstNodeRef node, const SelectorList &selectorsToPush)
{
    ContextCleanup cleanup(data);

    if (!selectorsToPush.isEmpty()) {
        data.selectors.push(selectorsToPush);
        cleanup.flags |= ContextCleanup::CleanupFlag::Selector;
    }

    if (!node.is_map()) {
        return cleanup;
    }

    with_child(node, "path", [&](auto node) {
        data.paths.push(value<QString>(node));
        cleanup.flags |= ContextCleanup::CleanupFlag::Path;
    });

    with_child(node, "prefix", [&](auto node) {
        data.prefixes.push(value<QString>(node));
        cleanup.flags |= ContextCleanup::CleanupFlag::Prefix;
    });

    with_child(node, "element", [&](auto node) {
        data.elementNames.push(value<QString>(node));
        cleanup.flags |= ContextCleanup::CleanupFlag::ElementName;
    });

    with_child(node, "colorSet", [&](auto node) {
        data.colorSets.push(value<Element::ColorSet>(node));
    });

    return cleanup;
}

SelectorList LoadingContext::selectors() const
{
    return data.selectors.isEmpty() ? SelectorList() : data.selectors.top();
}

QString LoadingContext::prefixedElementName() const
{
    if (data.elementNames.isEmpty()) {
        return QString{};
    }

    auto element = data.elementNames.top();
    if (!data.prefixes.isEmpty()) {
        auto prefix = data.prefixes.top();
        if (!prefix.isEmpty()) {
            element = prefix + u"-" + element;
        }
    }

    return element;
}

QString LoadingContext::path() const
{
    return data.paths.isEmpty() ? QString{} : data.paths.top();
}

Element::ColorSet LoadingContext::colorSet() const
{
    return data.colorSets.isEmpty() ? Element::ColorSet::None : data.colorSets.top();
}
