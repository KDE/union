// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "StyleCache_p.h"

#include <QFile>
#include <QSaveFile>
#include <QStandardPaths>

#include "QDataStreamExtras.h"
#include "Style_p.h"

#include "union_logging.h"

using namespace Union;
using namespace std::string_literals;

namespace fs = std::filesystem;

// Magic value to indicate the file is a proper Union cache file.
// Matches this ascii:                   #  U  N  I  O  N  C  F
static constexpr quint64 CacheMagic = 0x23'55'4E'49'4F'55'43'46;
// Version of the cache file. Increase this whenever there are changes to the
// underlying data structures.
static constexpr uint32_t CacheVersion = 1;

class StyleCache::Private
{
public:
    QHash<StyleId, fs::path> stylePaths;
};

StyleCache::StyleCache()
    : d(std::make_unique<Private>())
{
    auto cachePath = fs::path(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation).toStdString()) / "union";
    if (!enabled() || !fs::exists(cachePath)) {
        return;
    }

    for (auto pluginEntry : fs::directory_iterator(cachePath)) {
        if (!pluginEntry.is_directory()) {
            continue;
        }

        auto pluginName = pluginEntry.path().filename();
        for (auto styleEntry : fs::directory_iterator(pluginEntry.path())) {
            if (!styleEntry.is_regular_file()) {
                continue;
            }

            if (styleEntry.file_size() == 0) {
                continue;
            }

            auto styleName = styleEntry.path().stem();

            auto styleId = std::make_pair(pluginName.string(), styleName.string());
            d->stylePaths.insert(styleId, styleEntry.path());
        }
    }
}

StyleCache::~StyleCache() = default;

bool Union::StyleCache::enabled() const
{
    static bool enabled = !qEnvironmentVariableIsSet("UNION_DISABLE_STYLE_CACHE");
    return enabled;
}

bool StyleCache::hasEntry(const StyleId &styleId) const
{
    return d->stylePaths.contains(styleId);
}

std::unique_ptr<StylePrivate> StyleCache::load(const StyleId &styleId) const
{
    if (!enabled()) {
        qCDebug(UNION_GENERAL) << "Ignoring cache for style" << styleId.second << "from plugin" << styleId.first << "because caching has been disabled";
        return nullptr;
    }

    auto path = d->stylePaths.value(styleId);
    if (path.empty()) {
        qCDebug(UNION_GENERAL) << "Ignoring cache for style" << styleId.second << "from plugin" << styleId.first << "because no cache file could be found";
        return nullptr;
    }

    static bool forceRecreate = qEnvironmentVariableIsSet("UNION_FORCE_RECREATE_STYLE_CACHE");
    if (forceRecreate) {
        return nullptr;
    }

    QFile cacheFile(path);
    if (!cacheFile.open(QIODevice::ReadOnly)) {
        return nullptr;
    }

    QDataStream reader(&cacheFile);
    reader.setVersion(QDataStream::Qt_6_9);

    quint64 magic = 0;
    reader >> magic;

    if (magic != CacheMagic) {
        qCDebug(UNION_GENERAL) << "Ignoring cache file" << path.string() << "invalid magic value";
        return nullptr;
    }

    uint32_t version;
    reader >> version;

    if (version != CacheVersion) {
        qCDebug(UNION_GENERAL) << "Ignoring cache file" << path.string() << "version mismatch";
        return nullptr;
    }

    auto result = std::make_unique<StylePrivate>();
    reader >> result->pluginName;
    reader >> result->styleName;

    if (result->pluginName.toStdString() != styleId.first || result->styleName.toStdString() != styleId.second) {
        qCDebug(UNION_GENERAL) << "Ignoring cache file" << path.string() << "plugin/style name mismatch";
        return nullptr;
    }

    reader >> result->cachePaths;
    reader >> result->modificationTimes;

    if (result->cachePaths.size() != result->modificationTimes.size()) {
        qCDebug(UNION_GENERAL) << "Ignoring cache file" << path.string() << "mismatch between cache paths and modification times";
        return nullptr;
    }

    for (int i = 0; i < result->cachePaths.size(); ++i) {
        auto path = result->cachePaths.at(i);
        auto cachedModificationTime = result->modificationTimes.at(i);
        auto currentModificationTime = fs::last_write_time(path);

        if (cachedModificationTime != currentModificationTime) {
            qCDebug(UNION_GENERAL) << "Ignoring cache file" << path.string() << "file modification time mismatch";
            return nullptr;
        }
    }

    qsizetype count;
    reader >> count;

    for (int i = 0; i < count; ++i) {
        auto rule = StyleRule::create();
        reader >> rule;
        result->rules.append(rule);
    }

    if (reader.status() != QDataStream::Status::Ok) {
        qCDebug(UNION_GENERAL) << "Ignoring cache file" << path.string() << "restoring cached data failed";
        return nullptr;
    }

    return result;
}

bool Union::StyleCache::save(const StylePrivate *style) const
{
    if (!enabled()) {
        return false;
    }

    if (style->hasErrors) {
        return false;
    }

    auto path = fs::path(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation).toStdString()) / "union" / style->pluginName.toStdString();
    if (!fs::exists(path)) {
        if (!fs::create_directories(path)) {
            qCWarning(UNION_GENERAL) << "Could not create cache path" << path.string();
            return false;
        }
    }

    QSaveFile cacheFile(QString::fromStdString(path / (style->styleName.toStdString() + ".cache"s)));
    if (!cacheFile.open(QIODevice::WriteOnly)) {
        qCWarning(UNION_GENERAL) << "Could not open cache file" << qPrintable(cacheFile.fileName()) << "for writing";
        return false;
    }

    QDataStream writer(&cacheFile);
    writer.setVersion(QDataStream::Qt_6_9);

    writer << CacheMagic;
    writer << CacheVersion;

    writer << style->pluginName;
    writer << style->styleName;

    writer << style->cachePaths;
    writer << style->modificationTimes;

    writer << style->rules.size();
    for (const auto &rule : std::as_const(style->rules)) {
        writer << rule;
    }

    if (!cacheFile.commit()) {
        qCWarning(UNION_GENERAL) << "Could not commit cache file" << qPrintable(cacheFile.fileName());
        return false;
    }

    return true;
}
