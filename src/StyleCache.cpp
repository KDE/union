// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "StyleCache_p.h"

#include <QCryptographicHash>
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
static constexpr uint32_t CacheVersion = 6;

class StyleCache::Private
{
public:
    fs::path cachePath(const fs::path &stylePath)
    {
        auto pathHash = QCryptographicHash::hash(stylePath.string(), QCryptographicHash::Sha256).toBase64().toStdString();
        return storagePath / pathHash;
    }

    fs::path storagePath;
};

StyleCache::StyleCache()
    : d(std::make_unique<Private>())
{
    d->storagePath = fs::path(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation).toStdString()) / "union";
}

StyleCache::~StyleCache() = default;

bool Union::StyleCache::enabled() const
{
    static bool enabled = !qEnvironmentVariableIsSet("UNION_DISABLE_STYLE_CACHE");
    return enabled;
}

bool StyleCache::hasEntry(const fs::path &path) const
{
    return fs::exists(d->cachePath(path));
}

std::unique_ptr<StylePrivate> StyleCache::load(const fs::path &path) const
{
    if (!enabled()) {
        qCDebug(UNION_GENERAL) << "Ignoring cache for style" << path.string() << "because caching has been disabled";
        return nullptr;
    }

    auto cachePath = d->cachePath(path);
    if (!fs::exists(cachePath)) {
        qCDebug(UNION_GENERAL) << "Ignoring cache for style" << path.string() << "because no cache file could be found";
        return nullptr;
    }

    static bool forceRecreate = qEnvironmentVariableIsSet("UNION_FORCE_RECREATE_STYLE_CACHE");
    if (forceRecreate) {
        return nullptr;
    }

    QFile cacheFile(cachePath);
    if (!cacheFile.open(QIODevice::ReadOnly)) {
        return nullptr;
    }

    QDataStream reader(&cacheFile);
    reader.setVersion(QDataStream::Qt_6_9);

    quint64 magic = 0;
    reader >> magic;

    if (magic != CacheMagic) {
        qCDebug(UNION_GENERAL) << "Ignoring cache for style" << path.string() << "invalid magic value";
        return nullptr;
    }

    uint32_t version;
    reader >> version;

    if (version != CacheVersion) {
        qCDebug(UNION_GENERAL) << "Ignoring cache for style" << path.string() << "version mismatch";
        return nullptr;
    }

    auto result = std::make_unique<StylePrivate>();
    reader >> result->path;

    if (result->path != path) {
        qCDebug(UNION_GENERAL) << "Ignoring cache for style" << path.string() << "style path mismatch";
        return nullptr;
    }

    reader >> result->cachePaths;
    reader >> result->modificationTimes;

    if (result->cachePaths.size() != result->modificationTimes.size()) {
        qCDebug(UNION_GENERAL) << "Ignoring cache for style" << path.string() << "mismatch between cache paths and modification times";
        return nullptr;
    }

    for (int i = 0; i < result->cachePaths.size(); ++i) {
        auto path = result->cachePaths.at(i);

        if (!fs::exists(path)) {
            qCDebug(UNION_GENERAL) << "Ignoring cache for style" << path.string() << "original file no longer exists";
            return nullptr;
        }

        auto cachedModificationTime = result->modificationTimes.at(i);
        auto currentModificationTime = fs::last_write_time(path);

        if (cachedModificationTime != currentModificationTime) {
            qCDebug(UNION_GENERAL) << "Ignoring cache for style" << path.string() << "file modification time mismatch";
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
        qCDebug(UNION_GENERAL) << "Ignoring cache for style" << path.string() << "restoring cached data failed";
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

    if (!fs::exists(d->storagePath)) {
        if (!fs::create_directories(d->storagePath)) {
            qCWarning(UNION_GENERAL) << "Could not create cache path" << d->storagePath.string();
            return false;
        }
    }

    QSaveFile cacheFile(QString::fromStdString(d->cachePath(style->path)));
    if (!cacheFile.open(QIODevice::WriteOnly)) {
        qCWarning(UNION_GENERAL) << "Could not open cache file" << qPrintable(cacheFile.fileName()) << "for writing";
        return false;
    }

    QDataStream writer(&cacheFile);
    writer.setVersion(QDataStream::Qt_6_9);

    writer << CacheMagic;
    writer << CacheVersion;

    writer << style->path;

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
