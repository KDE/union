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

bool Theme::load()
{
    Q_ASSERT_X(d->loader, "Union::Theme", "Theme requires a ThemeLoader instance to function");
    return d->loader->load(shared_from_this());
}

void Theme::insert(StyleRule::Ptr style)
{
    qCDebug(UNION_GENERAL) << "Insert" << style;
    d->styles.append(style);
}

QList<StyleRule::Ptr> Theme::rules()
{
    return d->styles;
}

QList<StyleRule::Ptr> Union::Theme::matches(const QList<Element::Ptr> &elements)
{
    QList<StyleRule::Ptr> result;

    for (auto style : d->styles) {
        if (selectorListMatches(style->selectors(), elements)) {
            result.prepend(style);
        }
    }

    return result;
}

std::shared_ptr<Theme> Theme::create(const QString &pluginName, const QString &themeName, std::unique_ptr<ThemeLoader> &&loader)
{
    return std::make_shared<Theme>(std::make_unique<ThemePrivate>(pluginName, themeName, std::move(loader)));
}
