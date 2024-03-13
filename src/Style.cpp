/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Style.h"

#include <filesystem>

#include <QCoreApplication>
#include <QPluginLoader>
#include <QUrl>

#include "ElementIdentifier.h"
#include "InputPlugin.h"
#include "StyleLoader.h"

#include "union_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

class UNION_NO_EXPORT Style::Private
{
public:
    std::unordered_map<QString, std::unique_ptr<StyleLoader>> styleLoaders;

    std::unordered_map<ElementIdentifier, std::shared_ptr<StyleElement>> elements;
};

Style::Style()
    : QObject(nullptr)
    , d(std::make_unique<Private>())
{
}

Style::~Style() = default;

void Style::load()
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
            const auto name = metaData.value(u"union-pluginname").toString();

            if (metaData.value(u"union-plugintype").toString() == u"input"_qs) {
                auto inputPlugin = static_cast<InputPlugin *>(loader.instance());
                if (inputPlugin) {
                    qCDebug(UNION_GENERAL) << "Loaded input plugin" << name;
                    auto loader = inputPlugin->createStyleLoader(instance());
                    if (!loader->load()) {
                        qCWarning(UNION_GENERAL) << "Style loader failed to load";
                    } else {
                        d->styleLoaders.insert(std::make_pair(name, std::move(loader)));
                    }
                } else {
                    qCWarning(UNION_GENERAL) << "Failed loading plugin" << name << loader.errorString();
                }
            }
        }
    }
}

std::shared_ptr<StyleElement> Style::get(const ElementIdentifier &selector)
{
    auto element = d->elements.find(selector);
    if (element != d->elements.end()) {
        return element->second;
    }

    auto loader = d->styleLoaders.find(selector.style());
    if (loader == d->styleLoaders.end()) {
        return nullptr;
    }

    if (!loader->second->loadElement(selector)) {
        return nullptr;
    }

    element = d->elements.find(selector);
    if (element != d->elements.end()) {
        return element->second;
    } else {
        qCDebug(UNION_GENERAL) << "Could not find an element matching identifier" << selector;
        return nullptr;
    }
}

void Style::insert(const ElementIdentifier &identifier, std::shared_ptr<StyleElement> element)
{
    auto pair = std::make_pair(identifier, element);
    d->elements.insert(pair);
}

std::shared_ptr<Style> Style::instance()
{
    static std::shared_ptr<Style> inst = std::make_shared<Style>();
    return inst;
}

