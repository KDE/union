// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>
#include <optional>

#include <QCoreApplication>
#include <QJsonObject>
#include <QPluginLoader>

#include "union_export.h"
#include "union_logging.h"

namespace Union
{

/*!
 * \class Union::PluginRegistry
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief Template class to simplify looking up and loading plugins.
 */
template<typename T>
class UNION_EXPORT PluginRegistry
{
public:
    struct PluginInfo {
        std::filesystem::path path;
        QString name;
        QJsonObject metaData;
    };

    PluginRegistry(const QJsonObject &matchMetaData)
        : m_matchMetaData(matchMetaData)
    {
        findAllPlugins();
    }

    ~PluginRegistry()
    {
        m_plugins.clear();
        m_pluginObjects.clear();
    }

    std::optional<PluginInfo> pluginInfo(const QString &pluginName) const
    {
        auto itr = std::ranges::find(m_plugins, pluginName, &PluginInfo::name);
        if (itr != m_plugins.end()) {
            return *itr;
        }
        return std::nullopt;
    }

    QList<PluginInfo> plugins() const
    {
        return m_plugins;
    }

    T *pluginObject(const QString &pluginName)
    {
        if (m_pluginObjects.contains(pluginName)) {
            return m_pluginObjects.value(pluginName);
        }

        auto info = pluginInfo(pluginName);
        if (info) {
            return loadPlugin(info.value().path);
        }

        return nullptr;
    }

    QList<QString> pluginObjects() const
    {
        return m_pluginObjects.keys();
    }

private:
    void findAllPlugins()
    {
        const auto pluginDirs = QCoreApplication::libraryPaths();
        for (const auto &dir : pluginDirs) {
            const auto path = std::filesystem::path(dir.toStdString()) / "union";

            if (!std::filesystem::exists(path)) {
                continue;
            }

            for (const auto &entry : std::filesystem::directory_iterator(path)) {
                QPluginLoader loader(QString::fromStdString(entry.path()));
                const auto metaData = loader.metaData().value(u"MetaData").toObject();

                if (metaDataMatch(metaData, m_matchMetaData)) {
                    m_plugins.append(PluginInfo{.path = entry, .name = QString::fromStdString(entry.path().stem()), .metaData = metaData});
                }
            }
        }
    }

    T *loadPlugin(const std::filesystem::path &path)
    {
        QPluginLoader loader(QString::fromStdString(path));

        const auto metaData = loader.metaData().value(u"MetaData").toObject();
        const auto name = path.stem().string();

        if (!metaDataMatch(metaData, m_matchMetaData)) {
            qCWarning(UNION_GENERAL) << "Tried to load plugin" << name << "from" << path.string()
                                     << "which does not match metadata criteria:" << m_matchMetaData;
            return nullptr;
        }

        auto plugin = static_cast<T *>(loader.instance());
        if (!plugin) {
            qCWarning(UNION_GENERAL).nospace() << "Failed loading plugin " << name << " from " << path.string() << ": " << loader.errorString();
            return nullptr;
        }

        qCDebug(UNION_GENERAL) << "Loaded plugin" << name << "from" << path.string();
        m_pluginObjects.insert(QString::fromStdString(name), plugin);
        return plugin;
    }

    bool metaDataMatch(const QJsonObject &metaData, const QJsonObject &match)
    {
        if (metaData.isEmpty()) {
            return false;
        }

        for (auto itr = match.begin(); itr != match.end(); ++itr) {
            if (metaData.value(itr.key()) != itr.value()) {
                return false;
            }
        }

        return true;
    }

    QJsonObject m_matchMetaData;
    QList<PluginInfo> m_plugins;
    QHash<QString, T *> m_pluginObjects;
};

}
