// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "ThemeRegistry.h"

#include <filesystem>

#include <QCoreApplication>
#include <QPluginLoader>
#include <QThread>

#include "InputPlugin.h"
#include "PluginRegistry.h"
#include "Theme.h"

#include "union_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

class Union::ThemeRegistryPrivate
{
public:
    ThemeRegistryPrivate()
        : pluginRegistry{std::make_shared<PluginRegistry<InputPlugin>>(QJsonObject{{u"union-plugintype"_s, u"input"_s}})}
    {
    }

    Theme::Ptr loadTheme(const QString &themeName, const QString &pluginName)
    {
        auto themeId = qMakePair(pluginName, themeName);
        if (themes.contains(themeId)) {
            return themes.value(themeId);
        }

        auto plugin = pluginRegistry->pluginObject(pluginName);
        if (!plugin) {
            qCWarning(UNION_GENERAL) << "Requested theme" << themeName << "from plugin" << pluginName << "but the plugin could not be found!";
            return nullptr;
        }

        auto theme = plugin->createTheme(themeName);
        if (!theme) {
            qCWarning(UNION_GENERAL) << "Requested theme" << themeName << "from plugin" << pluginName << "but the theme could not be found!";
            return nullptr;
        }

        if (!theme->load()) {
            qCWarning(UNION_GENERAL) << "Requested theme" << themeName << "from plugin" << pluginName << "but it failed to load!";
            return nullptr;
        }

        themes.insert(themeId, theme);
        return theme;
    }

    std::shared_ptr<PluginRegistry<InputPlugin>> pluginRegistry;
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
    static auto name = qEnvironmentVariable("UNION_STYLE_NAME", u"default"_s);
    static auto plugin = qEnvironmentVariable("UNION_STYLE_PLUGIN", u"union-input-css"_s);
    return theme(name, plugin);
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
    const auto objects = d->pluginRegistry->pluginObjects();
    for (const auto &object : objects) {
        if (auto theme = d->loadTheme(themeName, object); theme) {
            return theme;
        }
    }

    // Nothing found in loaded plugins, try and load each available plugin and
    // see if that returns something.
    const auto plugins = d->pluginRegistry->plugins();
    for (const auto &plugin : plugins) {
        if (auto theme = d->loadTheme(themeName, plugin.name); theme) {
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
    instance->d->pluginRegistry.reset();
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
