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
#include "StyleLoader.h"

#include "union_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

static void loadThemeOnStartup()
{
    if (QThread::currentThread() == QCoreApplication::instance()->thread()) {
        Theme::instance()->load();
    }

    // If we are not running on the main thread, which may happen if we're being
    // loaded by QML, ensure we only create and load the Theme instance on the
    // main thread by using a QTimer that we explicitly move to the main thread.
    QTimer *mainThreadTimer = new QTimer();
    mainThreadTimer->moveToThread(QCoreApplication::instance()->thread());
    mainThreadTimer->setSingleShot(true);
    QObject::connect(
        mainThreadTimer,
        &QTimer::timeout,
        mainThreadTimer,
        [mainThreadTimer]() {
            Theme::instance()->load();
            mainThreadTimer->deleteLater();
        },
        Qt::QueuedConnection);
    // The timer is now on the main thread so it needs to be started from there.
    QMetaObject::invokeMethod(mainThreadTimer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
}
Q_COREAPP_STARTUP_FUNCTION(loadThemeOnStartup)

class Union::ThemePrivate
{
public:
    std::unordered_map<QString, std::unique_ptr<StyleLoader>> styleLoaders;

    QList<Style::Ptr> styles;
};

Theme::Theme(std::unique_ptr<ThemePrivate> &&d)
    : QObject(nullptr)
    , d(std::move(d))
{
}

Theme::~Theme() = default;

void Theme::load()
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
                    if (!loader || !loader->load()) {
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

void Theme::insert(Style::Ptr style)
{
    qCDebug(UNION_GENERAL) << "Insert style" << style;
    d->styles.append(style);
}

QList<Style::Ptr> Union::Theme::matches(const QList<Element::Ptr> &elements)
{
    QList<Style::Ptr> result;

    for (auto style : d->styles) {
        if (selectorListMatches(style->selectors(), elements)) {
            result.prepend(style);
        }
    }

    return result;
}

std::shared_ptr<Theme> Theme::instance()
{
    static std::shared_ptr<Theme> inst;
    if (!inst) {
        if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
            qCFatal(UNION_GENERAL) << "Theme cannot be instantiated on a different thread than the main thread";
        }
        inst = std::make_shared<Theme>(std::make_unique<ThemePrivate>());
    }
    return inst;
}
