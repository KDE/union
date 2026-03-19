// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>
#include <memory>

#include <QList>

#include "StyleRule.h"

#include "union_export.h"

namespace Union
{
class StylePrivate;

/*
 * A cache for data of Style instances.
 *
 * This serializes and deserializes Style data from a file on disk using
 * QDataStream.
 *
 * Note that if `enabled()` returns false, this class will do nothing.
 */
class StyleCache
{
public:
    using StyleId = std::pair<std::string, std::string>;

    StyleCache();
    ~StyleCache();

    // Is the cache enabled.
    //
    // Primarly checks if the UNION_DISABLE_STYLE_CACHE environment variable is
    // set.
    bool enabled() const;

    // Does a cache entry exist for the given style ID.
    //
    // Note that this is based on whether a cache file exists for the given ID.
    // It does not verify that the cache file actually loads properly, so this
    // may return true while load() still ends up returning nullptr.
    bool hasEntry(const StyleId &styleId) const;

    // Load the cached data for the style with the given style ID.
    //
    // This will return nullptr when the cached data cannot be loaded. Cached
    // data may not be loaded for a number of reasons, including changes to the
    // underlying style files as well as changes to the code or structure of
    // cache files.
    std::unique_ptr<StylePrivate> load(const StyleId &styleId) const;

    // Save the style data to a cache file.
    //
    // This will serialize the given style data to a file in an appropriate
    // cache location on disk.
    bool save(const StylePrivate *style) const;

private:
    class Private;
    const std::unique_ptr<Private> d;
};

}
