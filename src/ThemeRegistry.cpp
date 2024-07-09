/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "ThemeRegistry.h"

#include <filesystem>

#include <QCoreApplication>
#include <QPluginLoader>
#include <QThread>

#include "InputPlugin.h"
#include "Theme.h"

#include "union_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

struct PluginInfo {
    std::filesystem::path path;
    QString name;
    QJsonObject metaData;
};

static const QJsonObject inputPluginFilter{{u"union-plugintype"_s, u"input"_s}};

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

class Union::ThemeRegistryPrivate
{
public:
    QList<PluginInfo> findAllPlugins(const QJsonObject &matchMetaData = QJsonObject{})
    {
        QList<PluginInfo> plugins;

        const auto pluginDirs = QCoreApplication::libraryPaths();
        for (const auto &dir : pluginDirs) {
            const auto path = std::filesystem::path(dir.toStdString()) / "union";

            if (!std::filesystem::exists(path)) {
                continue;
            }

            for (const auto &entry : std::filesystem::directory_iterator(path)) {
                QPluginLoader loader(QString::fromStdString(entry.path()));
                const auto metaData = loader.metaData().value(u"MetaData").toObject();

                if (metaDataMatch(metaData, matchMetaData)) {
                    plugins.append(PluginInfo{.path = entry, .name = metaData.value(u"union-pluginname").toString(), .metaData = metaData});
                }
            }
        }

        return plugins;
    }

    InputPlugin *loadPlugin(const std::filesystem::path &path, const QJsonObject &matchMetadata = QJsonObject{})
    {
        QPluginLoader loader(QString::fromStdString(path));
        const auto metaData = loader.metaData().value(u"MetaData").toObject();
        const auto name = metaData.value(u"union-pluginname").toString();

        if (!metaDataMatch(metaData, matchMetadata)) {
            qCWarning(UNION_GENERAL) << "Tried to load plugin" << name << "from" << path.string() << "which does not match metadata criteria:" << matchMetadata;
            return nullptr;
        }

        auto plugin = static_cast<InputPlugin *>(loader.instance());
        if (!plugin) {
            qCWarning(UNION_GENERAL).nospace() << "Failed loading plugin " << name << " from " << path.string() << ": " << loader.errorString();
            return nullptr;
        }

        qCDebug(UNION_GENERAL) << "Loaded input plugin" << name << "from" << path.string();
        inputPlugins.insert(name, plugin);
        return plugin;
    }

    InputPlugin *loadInputPlugin(const QString &pluginName)
    {
        if (inputPlugins.contains(pluginName)) {
            return inputPlugins.value(pluginName);
        }

        const auto plugins = findAllPlugins(inputPluginFilter);
        for (const auto &info : plugins) {
            if (info.name == pluginName) {
                return loadPlugin(info.path, inputPluginFilter);
            }
        }

        return nullptr;
    }

    Theme::Ptr loadTheme(const QString &themeName, const QString &pluginName)
    {
        auto themeId = qMakePair(pluginName, themeName);
        if (themes.contains(themeId)) {
            return themes.value(themeId);
        }

        auto plugin = loadInputPlugin(pluginName);
        if (!plugin) {
            qCWarning(UNION_GENERAL) << "Requested theme" << themeName << "from plugin" << pluginName << "but the plugin could not be found!";
            return nullptr;
        }

        // TODO Implement actual plugin loading
        return nullptr;
    }

    QHash<QString, InputPlugin *> inputPlugins;
    QHash<QPair<QString, QString>, std::shared_ptr<Theme>> themes;
};

ThemeRegistry::ThemeRegistry(std::unique_ptr<ThemeRegistryPrivate> &&d)
    : QObject(nullptr)
    , d(std::move(d))
{
    // Ensure we execute cleanup during QCoreApplication shutdown instead of
    // static variable cleanup so that any resources that require a valid
    // application are handled properly.
    qAddPostRoutine(ThemeRegistry::cleanup);
}

ThemeRegistry::~ThemeRegistry() = default;

void ThemeRegistry::load()
{
    // Does nothing currently, themes and plugins are loaded on demand.
    // TODO: Load cached data.
}

void ThemeRegistry::save()
{
    // TODO: Implement caching of loaded themes.
}

std::shared_ptr<Theme> ThemeRegistry::defaultTheme()
{
    return theme(u"default"_s, u"plasmasvg"_s);
}

std::shared_ptr<Theme> ThemeRegistry::theme(const QString &themeName, const QString &pluginName)
{
    if (!pluginName.isEmpty()) {
        return d->loadTheme(themeName, pluginName);
    }

    // pluginName is empty so we don't know which exact input plugin provides
    // the theme. Search through all input plugins until we find one that
    // returns a valid theme for themeName.

    // First search through already-loaded plugins
    const auto pluginNames = d->inputPlugins.keys();
    for (const auto &name : pluginNames) {
        if (auto theme = d->loadTheme(themeName, name); theme) {
            return theme;
        }
    }

    // Nothing found in loaded plugins, try and load each available plugin and
    // see if that returns something.
    const auto allInputPlugins = d->findAllPlugins(inputPluginFilter);
    for (const auto &info : allInputPlugins) {
        if (auto theme = d->loadTheme(themeName, info.name); theme) {
            return theme;
        }
    }

    qCWarning(UNION_GENERAL) << "Requested theme" << themeName << "which could not be found in any plugin!";
    return nullptr;
}

void ThemeRegistry::cleanup()
{
    auto instance = ThemeRegistry::instance();

    instance->save();

    instance->d->themes.clear();
    instance->d->inputPlugins.clear();
}

std::shared_ptr<ThemeRegistry> ThemeRegistry::instance()
{
    static std::shared_ptr<ThemeRegistry> inst;
    if (!inst) {
        Q_ASSERT_X(QThread::currentThread() == QCoreApplication::instance()->thread(),
                   "Union::ThemeRegistry",
                   "ThemeLoader cannot be instantiated on a different thread than the main thread");
        inst = std::make_shared<ThemeRegistry>(std::make_unique<ThemeRegistryPrivate>());
    }
    return inst;
}
