// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "StyleRegistry.h"

#include <filesystem>

#include <QGuiApplication>
#include <QJsonArray>
#include <QPluginLoader>
#include <QThread>

#include "PluginRegistry.h"
#include "Style.h"
#include "StyleCache_p.h"
#include "Style_p.h"

#include "union_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

// This is implements the most minimal of PlatformPlugin that can be used as a
// fallback if we have no existing platform plugin for the current platform.
class FallbackPlatformPlugin : public PlatformPlugin
{
    QString defaultStyleName() override
    {
        return u"breeze"_s;
    }
};

class Union::StyleRegistryPrivate
{
public:
    StyleRegistryPrivate()
        : inputRegistry{std::make_shared<PluginRegistry<InputPlugin>>(u"input"_s)}
        , platformRegistry{std::make_shared<PluginRegistry<PlatformPlugin>>(u"platform"_s)}
    {
    }

    Style::Ptr loadStyle(const QString &styleName, const QString &pluginName)
    {
        auto styleId = std::make_pair(pluginName.toStdString(), styleName.toStdString());
        if (styles.contains(styleId)) {
            return styles.value(styleId);
        }

        if (styleCache->hasEntry(styleId)) {
            auto data = styleCache->load(styleId);
            if (data) {
                auto style = std::make_shared<Style>(std::move(data));
                qCDebug(UNION_GENERAL) << "Loaded style" << styleName << "from cached data.";
                styles.insert(styleId, style);
                return style;
            }
        }

        static const bool DisablePlugins = qEnvironmentVariableIsSet("UNION_DISABLE_INPUT_PLUGINS");
        if (DisablePlugins) {
            return nullptr;
        }

        auto plugin = inputRegistry->pluginObject(pluginName);
        if (!plugin) {
            qCWarning(UNION_GENERAL) << "Requested style" << styleName << "from plugin" << pluginName << "but the plugin could not be found!";
            return nullptr;
        }

        auto style = plugin->createStyle(styleName);
        if (!style) {
            qCWarning(UNION_GENERAL) << "Requested style" << styleName << "from plugin" << pluginName << "but the style could not be found!";
            return nullptr;
        }

        if (!style->load()) {
            qCWarning(UNION_GENERAL) << "Requested style" << styleName << "from plugin" << pluginName << "but it failed to load!";
            return nullptr;
        }

        styles.insert(styleId, style);
        return style;
    }

    void loadPlatform()
    {
        const auto forcedPlatform = qEnvironmentVariable("UNION_FORCE_PLATFORM", QString{});
        if (!forcedPlatform.isEmpty()) {
            platform = std::shared_ptr<PlatformPlugin>(platformRegistry->pluginObject(forcedPlatform));
            if (platform) {
                return;
            }
        }

        const auto platformName = QGuiApplication::platformName();
        const auto desktopNames = qEnvironmentVariable("XDG_CURRENT_DESKTOP", QString{}).split(u':');

        const auto plugins = platformRegistry->plugins();
        for (const auto &plugin : plugins) {
            const auto supportedPlatforms = plugin.metaData.value(u"union-supported-platforms").toArray().toVariantList();
            auto platformItr = std::ranges::find_if(supportedPlatforms, [platformName](const QVariant &platform) {
                return platformName.compare(platform.toString(), Qt::CaseInsensitive) == 0;
            });
            if (platformItr == supportedPlatforms.end()) {
                continue;
            }

            const auto supportedSessions = plugin.metaData.value(u"union-supported-sessions").toArray().toVariantList();
            if (!supportedSessions.isEmpty()) {
                auto sessionItr = std::ranges::find_if(supportedSessions, [&desktopNames](const QVariant &session) {
                    return desktopNames.contains(session.toString(), Qt::CaseSensitivity::CaseInsensitive);
                });
                if (sessionItr == supportedSessions.end()) {
                    continue;
                }
            }

            platform = std::shared_ptr<PlatformPlugin>(platformRegistry->pluginObject(plugin.name));
            if (platform) {
                break;
            }
        }

        if (!platform) {
            qCCritical(UNION_GENERAL) << "Could not find a supported platform plugin for platform" << platformName << "and session names" << desktopNames;
            qCCritical(UNION_GENERAL) << "Creating a default fallback platform plugin";
            platform = std::make_shared<FallbackPlatformPlugin>();
        }
    }

    std::unique_ptr<StyleCache> styleCache;

    std::shared_ptr<PluginRegistry<InputPlugin>> inputRegistry;
    QHash<StyleCache::StyleId, std::shared_ptr<Style>> styles;

    std::shared_ptr<PluginRegistry<PlatformPlugin>> platformRegistry;
    std::shared_ptr<PlatformPlugin> platform;
};

StyleRegistry::StyleRegistry(std::unique_ptr<StyleRegistryPrivate> &&d)
    : QObject(nullptr)
    , d(std::move(d))
{
    // Ensure we execute cleanup during QCoreApplication shutdown instead of
    // static variable cleanup so that any resources that require a valid
    // application are handled properly.
    qAddPostRoutine(StyleRegistry::cleanup);
}

StyleRegistry::~StyleRegistry() = default;

void StyleRegistry::load()
{
    d->styleCache = std::make_unique<StyleCache>();
}

void StyleRegistry::save()
{
    for (const auto &style : std::as_const(d->styles)) {
        if (style->d->modified) {
            d->styleCache->save(style->d.get());
        }
    }
}

std::shared_ptr<Style> StyleRegistry::defaultStyle()
{
    static auto environmentPlugin = qEnvironmentVariable("UNION_STYLE_PLUGIN", QString{});
    static auto environmentName = qEnvironmentVariable("UNION_STYLE_NAME", QString{});

    auto plugin = environmentPlugin;
    if (plugin.isEmpty()) {
        plugin = platform()->defaultInputPlugin();
    }

    auto name = environmentName;
    if (name.isEmpty()) {
        name = platform()->defaultStyleName();
    }

    return style(name, plugin);
}

std::shared_ptr<Style> StyleRegistry::style(const QString &styleName, const QString &pluginName)
{
    if (!pluginName.isEmpty()) {
        return d->loadStyle(styleName, pluginName);
    }

    // pluginName is empty so we don't know which exact input plugin provides
    // the style. Search through all input plugins until we find one that
    // returns a valid style for styleName.

    // First search through already-loaded plugins
    const auto objects = d->inputRegistry->pluginObjects();
    for (const auto &object : objects) {
        if (auto style = d->loadStyle(styleName, object); style) {
            return style;
        }
    }

    // Nothing found in loaded plugins, try and load each available plugin and
    // see if that returns something.
    const auto plugins = d->inputRegistry->plugins();
    for (const auto &plugin : plugins) {
        if (auto style = d->loadStyle(styleName, plugin.name); style) {
            return style;
        }
    }

    qCWarning(UNION_GENERAL) << "Requested style" << styleName << "which could not be found in any plugin!";
    return nullptr;
}

void Union::StyleRegistry::addStyle(const std::shared_ptr<Style> &style)
{
    auto styleId = std::make_pair(style->pluginName().toStdString(), style->name().toStdString());
    if (d->styles.contains(styleId)) {
        qCWarning(UNION_GENERAL) << "A style from plugin" << style->pluginName() << "with name" << style->name() << "is already registered";
        return;
    }

    d->styles.insert(styleId, style);
}

std::shared_ptr<PlatformPlugin> Union::StyleRegistry::platform() const
{
    if (!d->platform) {
        d->loadPlatform();
    }

    return d->platform;
}

void StyleRegistry::cleanup()
{
    auto instance = StyleRegistry::instance();

    instance->save();

    instance->d->styles.clear();
    instance->d->inputRegistry.reset();

    instance->d->platform.reset();
    instance->d->platformRegistry.reset();
}

std::shared_ptr<StyleRegistry> StyleRegistry::instance()
{
    static std::shared_ptr<StyleRegistry> inst;
    if (!inst) {
        Q_ASSERT_X(QThread::currentThread() == QCoreApplication::instance()->thread(),
                   "Union::StyleRegistry",
                   "StyleRegistry cannot be instantiated on a different thread than the main thread");
        inst = std::make_shared<StyleRegistry>(std::make_unique<StyleRegistryPrivate>());
    }
    return inst;
}

#include "moc_StyleRegistry.cpp"
