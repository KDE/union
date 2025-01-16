// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QPalette>
#include <QStack>

#include <ryml.hpp>

#include <Element.h>
#include <Selector.h>
#include <Theme.h>

#include "plasmasvg_logging.h"

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
    QStack<QPalette::ColorGroup> colorGroups;
    QStack<QByteArrayView> propertyNames;
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
        PropertyName = 1 << 5,
        ColorGroup = 1 << 6,
    };
    Q_DECLARE_FLAGS(CleanupFlags, CleanupFlag)

    ContextCleanup(ContextData &_data);
    ~ContextCleanup() noexcept;

    ContextData &data;
    CleanupFlags flags;
};

struct LoadingContext {
    LoadingContext(Union::Theme::Ptr _theme, ryml::Parser *_parser);

    Union::Theme::Ptr theme;
    ryml::Parser *parser;
    ContextData data;

    [[nodiscard]] ContextCleanup pushFromNode(ryml::ConstNodeRef node);
    [[nodiscard]] ContextCleanup pushFromNode(ryml::ConstNodeRef node, const Union::SelectorList &selectorsToPush);
    [[nodiscard]] ContextCleanup pushFromNode(ryml::ConstNodeRef node, QByteArrayView propertyName);

    inline QDebug logLocation(ryml::ConstNodeRef node) const
    {
        auto loc = parser->location(node);
        return (QMessageLogger(nullptr, 0, nullptr).warning(UNION_PLASMASVG).nospace().noquote() << loc.name.data() << "@" << loc.line << ":").space().quote();
    }

    Union::SelectorList selectors() const;
    QString prefixedElementName() const;
    QString path() const;
    Union::Element::ColorSet colorSet() const;
    QPalette::ColorGroup colorGroup() const;
    QByteArrayView propertyName() const;
};
