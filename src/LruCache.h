// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>
#include <list>
#include <optional>
#include <unordered_map>

#include <QImage>

#include "union_export.h"

namespace Union
{

/*!
 * \class Union::LruCache
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief A simple least-recently-used cache template.
 *
 * This will cache up to \p MaxSize values. If the maximum size is reached,
 * inserting a new item will evict the least recently used item from the cache.
 *
 * Values are expected to be copyable types. Do not use this to store raw
 * pointers, instead use some form of managed pointer when storing
 * heap-allocated data.
 */
template<typename Key, typename Value, std::size_t MaxSize = 100>
class LruCache
{
public:
    static_assert(MaxSize > 0, "LruCache should have a maximum size > 0");

    LruCache()
    {
    }

    /*!
     * Returns if the cache contains the specified key.
     */
    bool contains(const Key &key)
    {
        return m_values.contains(key);
    }

    /*!
     * Returns the value associated with a given key, if the key is in the cache.
     *
     * If not, it will return \c{std::nullopt}. Using this function will update
     * the given value to be the most recently used value.
     */
    std::optional<Value> value(const Key &key)
    {
        auto itr = m_values.find(key);
        if (itr == m_values.end()) {
            return std::nullopt;
        }

        m_usageTracker.splice(m_usageTracker.end(), m_usageTracker, (*itr).second.second);
        return (*itr).second.first;
    }

    /*!
     * Insert a new value associated with the given key into the cache.
     *
     * If the cache already contains a value matching the given key, it will
     * replace the existing value.
     */
    void insert(const Key &key, const Value &value)
    {
        auto itr = m_values.find(key);
        if (itr == m_values.end()) {
            // Maximum size reached, drop a value before inserting a new one.
            if (m_usageTracker.size() == MaxSize) {
                auto frontItr = m_values.find(m_usageTracker.front());
                m_values.erase(frontItr);
                m_usageTracker.pop_front();
            }

            auto usageItr = m_usageTracker.insert(m_usageTracker.end(), key);
            m_values.insert(std::make_pair(key, std::make_pair(value, usageItr)));
        } else {
            auto usageItr = (*itr).second.second;
            m_values[key] = std::make_pair(value, usageItr);
            m_usageTracker.splice(m_usageTracker.end(), m_usageTracker, (*itr).second.second);
        }
    }

    /*!
     * Clear the cache of all entries.
     */
    void clear()
    {
        m_usageTracker.clear();
        m_values.clear();
    }

private:
    // List used to keep track of recent usage. Keys are inserted at the back,
    // or moved there if used. The front thus becomes the key that was least
    // recently used.
    std::list<Key> m_usageTracker;
    // Hash used to keep track of values. The iterator is used to keep track of
    // the position of the key in m_usageTracker.
    std::unordered_map<Key, std::pair<Value, typename std::list<Key>::iterator>> m_values;
};

/*!
 * \class Union::LruImageCache
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief An adaptation of LruCache for caching images.
 *
 * This adds a load() method that makes it simple to use for caching images.
 */
class LruImageCache : public LruCache<int, QImage>
{
public:
    /*!
     * Load an image at a given size.
     *
     * This will return the image from \p path at \p size. The image will be
     * cached and future calls to load() for the same path and size will return
     * the cached image.
     */
    QImage load(const std::filesystem::path &path, const QSizeF &size)
    {
        auto key = qHashMulti(QHashSeed::globalSeed(), path.string(), size.width(), size.height());
        if (contains(key)) {
            return value(key).value();
        }

        QImage image(size.toSize(), QImage::Format_ARGB32_Premultiplied);
        image.load(QString::fromStdString(path));
        insert(key, image);

        return image;
    }
};
}
