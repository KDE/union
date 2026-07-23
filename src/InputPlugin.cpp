// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "InputPlugin.h"

#include "PluginRegistry.h"

using namespace Union;
using namespace Qt::StringLiterals;

static QHash<QString, std::shared_ptr<InputPlugin>> s_inputPlugins;
static std::shared_ptr<PluginRegistry<InputPlugin>> s_inputRegistry;

InputPlugin::InputPlugin(QObject *parent)
    : Plugin(parent)
{
}

std::shared_ptr<InputPlugin> Union::InputPlugin::inputPlugin(const QString &typeName)
{
    auto inputPlugin = s_inputPlugins.value(typeName);

    static const auto disableInputPlugins = qEnvironmentVariableIsSet("UNION_DISABLE_INPUT_PLUGINS");
    if (!inputPlugin && !disableInputPlugins) {
        if (!s_inputRegistry) {
            s_inputRegistry = std::make_shared<PluginRegistry<InputPlugin>>(u"input"_s);
        }

        inputPlugin = std::shared_ptr<InputPlugin>(s_inputRegistry->pluginObject(typeName));
        if (inputPlugin) {
            s_inputPlugins.insert(typeName, inputPlugin);
        } else {
            return nullptr;
        }
    }

    return inputPlugin;
}

void InputPlugin::addInputType(const QString &typeName, const std::shared_ptr<InputPlugin> &plugin)
{
    if (s_inputPlugins.contains(typeName)) {
        qCWarning(UNION_GENERAL) << "Input type" << typeName << "already exits";
        return;
    }

    s_inputPlugins.insert(typeName, plugin);
}

#include "moc_InputPlugin.cpp"
