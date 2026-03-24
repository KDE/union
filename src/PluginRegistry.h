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

class UNION_EXPORT Plugin : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    inline std::filesystem::path path() const
    {
        return m_path;
    }

    inline QString type() const
    {
        return m_type;
    }

    inline QString name() const
    {
        return m_name;
    }

    inline QJsonObject metaData() const
    {
        return m_metaData;
    }

private:
    template<typename T>
        requires std::derived_from<T, Plugin>
    friend class PluginRegistry;

    std::filesystem::path m_path;
    QString m_type;
    QString m_name;
    QJsonObject m_metaData;
};

/*!
 * \class Union::PluginRegistry
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief Template class to simplify looking up and loading plugins.
 */
template<typename T>
    requires std::derived_from<T, Plugin>
class UNION_EXPORT PluginRegistry
{
public:
    struct PluginInfo {
        std::filesystem::path path;
        QString name;
        QJsonObject metaData;
    };

    PluginRegistry(const QString &type, const QJsonObject &matchMetaData = QJsonObject{})
        : m_type(type)
        , m_matchMetaData(matchMetaData)
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
                const auto type = metaData.value(u"union-plugintype").toString();
                const auto name = metaData.value(u"union-pluginname").toString();

                if (type == m_type && metaDataMatch(metaData, m_matchMetaData)) {
                    m_plugins.append(PluginInfo{.path = entry, .name = name, .metaData = metaData});
                }
            }
        }
    }

    T *loadPlugin(const std::filesystem::path &path)
    {
        QPluginLoader loader(QString::fromStdString(path));

        const auto metaData = loader.metaData().value(u"MetaData").toObject();
        auto type = metaData.value(u"union-plugintype").toString();
        auto name = metaData.value(u"union-pluginname").toString();

        if (type != m_type) {
            qCWarning(UNION_GENERAL) << "Tried to load plugin" << name << "from" << path.string() << "which does not match type" << qPrintable(m_type);
            return nullptr;
        }

        if (!metaDataMatch(metaData, m_matchMetaData)) {
            qCWarning(UNION_GENERAL) << "Tried to load plugin" << name << "from" << path.string()
                                     << "which does not match metadata criteria:" << m_matchMetaData;
            return nullptr;
        }

        auto object = loader.instance();
        if (!object) {
            qCWarning(UNION_GENERAL).nospace() << "Failed loading plugin " << name << " from " << path.string() << ": " << loader.errorString();
            return nullptr;
        }

        auto plugin = static_cast<T *>(loader.instance());
        plugin->m_path = path;
        plugin->m_name = name;
        plugin->m_type = metaData.value(u"union-plugintype").toString();
        plugin->m_metaData = metaData;

        qCDebug(UNION_GENERAL) << "Loaded" << qPrintable(m_type) << "plugin" << name << "from" << path.string();
        m_pluginObjects.insert(name, plugin);

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

    QString m_type;
    QJsonObject m_matchMetaData;
    QList<PluginInfo> m_plugins;
    QHash<QString, T *> m_pluginObjects;
};

}
