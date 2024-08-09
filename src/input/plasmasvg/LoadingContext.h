/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QStack>

#include <ryml.hpp>

#include <Element.h>
#include <Selector.h>
#include <Theme.h>

/**
 * This file contains helpers that make it easier to work with contextual data
 * when loading a Plasma theme.
 */

/**
 * Main context data struct.
 */
struct ContextData {
    QStack<Union::SelectorList> selectors;
    QStack<QString> paths;
    QStack<QString> prefixes;
    QStack<QString> elementNames;
    QStack<Union::Element::ColorSet> colorSets;
};

/**
 * Scope guard that will cleanup the relevant items from ContextData when it gets destroyed.
 */
struct ContextCleanup {
    enum class CleanupFlag {
        Selector = 1 << 0,
        Path = 1 << 1,
        Prefix = 1 << 2,
        ElementName = 1 << 3,
        ColorSet = 1 << 4,
    };
    Q_DECLARE_FLAGS(CleanupFlags, CleanupFlag)

    ContextCleanup(ContextData &_data);
    ~ContextCleanup() noexcept;

    ContextData &data;
    CleanupFlags flags;
};

struct LoadingContext {
    LoadingContext(Union::Theme::Ptr _theme);

    Union::Theme::Ptr theme;
    ContextData data;

    [[nodiscard]] ContextCleanup pushFromNode(ryml::ConstNodeRef node, const Union::SelectorList &selectorsToPush = Union::SelectorList{});

    Union::SelectorList selectors() const;
    QString prefixedElementName() const;
    QString path() const;
    Union::Element::ColorSet colorSet() const;
};
