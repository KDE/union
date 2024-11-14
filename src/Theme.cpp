/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Theme.h"

#include <filesystem>

#include <QCoreApplication>
#include <QGlobalStatic>
#include <QMetaObject>
#include <QPluginLoader>
#include <QThread>
#include <QTimer>
#include <QUrl>

#include "InputPlugin.h"
#include "ThemeLoader.h"

#include "union_logging.h"
#include "union_query_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

class Union::ThemePrivate
{
public:
    ThemePrivate(const QString &_pluginName, const QString &_themeName, std::unique_ptr<ThemeLoader> &&_loader)
        : pluginName(_pluginName)
        , themeName(_themeName)
        , loader(std::move(_loader))
    {
    }

    QString pluginName;
    QString themeName;

    std::unique_ptr<ThemeLoader> loader;

    QList<StyleRule::Ptr> styles;
};

Theme::Theme(std::unique_ptr<ThemePrivate> &&d)
    : QObject(nullptr)
    , d(std::move(d))
{
}

Theme::~Theme() = default;

QString Theme::name() const
{
    return d->themeName;
}

QString Theme::pluginName() const
{
    return d->pluginName;
}

ThemeLoader *Theme::loader() const
{
    return d->loader.get();
}

bool Theme::load()
{
    Q_ASSERT_X(d->loader, "Union::Theme", "Theme requires a ThemeLoader instance to function");
    return d->loader->load(shared_from_this());
}

void Theme::insert(StyleRule::Ptr style)
{
    qCDebug(UNION_QUERY) << "Insert" << style;
    d->styles.append(style);
}

QList<StyleRule::Ptr> Theme::rules()
{
    return d->styles;
}

QList<StyleRule::Ptr> Union::Theme::matches(const QList<Element::Ptr> &elements)
{
    QList<StyleRule::Ptr> result;

    if (d->styles.isEmpty()) {
        qCDebug(UNION_QUERY) << "No style rules found for theme" << d->themeName << "so we will never match anything!";
    }

    for (auto style : d->styles) {
        const auto selectors = style->selectors();
        if (selectors.matches(elements)) {
            qCDebug(UNION_QUERY) << "Matches selector" << selectors;
            result.prepend(style);
        } else {
            qCDebug(UNION_QUERY) << "Does not match selectors" << selectors;
        }
    }

    return result;
}

std::shared_ptr<Theme> Theme::create(const QString &pluginName, const QString &themeName, std::unique_ptr<ThemeLoader> &&loader)
{
    return std::make_shared<Theme>(std::make_unique<ThemePrivate>(pluginName, themeName, std::move(loader)));
}
