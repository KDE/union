// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "Style.h"

#include <EventHelper.h>
#include <QCoreApplication>
#include <QGlobalStatic>
#include <QMetaObject>
#include <QPluginLoader>
#include <QThread>
#include <QTimer>
#include <QUrl>

#include "InputPlugin.h"
#include "StyleLoader.h"
#include "Style_p.h"

#include "union_logging.h"
#include "union_query_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

UNION_EXPORT QEvent::Type StyleChangedEvent::s_type = QEvent::None;
static EventTypeRegistration<StyleChangedEvent> styleChangedEventRegistration;

Style::Style(std::unique_ptr<StylePrivate> &&d)
    : QObject(nullptr)
    , d(std::move(d))
{
    qApp->installEventFilter(this);
}

Style::~Style() = default;

QString Style::name() const
{
    return d->styleName;
}

QString Style::pluginName() const
{
    return d->pluginName;
}

bool Style::hasErrors() const
{
    return d->hasErrors;
}

void Style::setHasErrors(bool newHasErrors)
{
    if (d->hasErrors != newHasErrors) {
        d->hasErrors = newHasErrors;
        Q_EMIT hasErrorsChanged();
    }
}

StyleLoader *Style::loader() const
{
    return d->loader.get();
}

bool Style::load()
{
    Q_ASSERT_X(d->loader, "Union::Style", "Style requires a StyleLoader instance to function");
    d->modified = true;
    return d->loader->load(shared_from_this());
}

void Style::addCachePath(const std::filesystem::path &path)
{
    if (!std::filesystem::exists(path)) {
        qCWarning(UNION_GENERAL) << "Ignoring non-existing cache path" << path.string();
    }

    d->cachePaths.append(path);
    d->modificationTimes.append(std::filesystem::last_write_time(path));
}

void Style::insert(StyleRule::Ptr style)
{
    qCInfo(UNION_QUERY) << "Insert" << style;
    d->rules.append(style);
}

QList<StyleRule::Ptr> Style::rules()
{
    return d->rules;
}

QList<StyleRule::Ptr> Union::Style::matches(const QList<Element::Ptr> &elements)
{
    QList<StyleRule::Ptr> result;

    if (d->rules.isEmpty()) {
        qCInfo(UNION_QUERY) << "No style rules found for theme" << d->styleName << "so we will never match anything!";
    }

    for (auto rule : d->rules) {
        const auto selectors = rule->selectors();
        if (selectors.matches(elements)) {
            qCDebug(UNION_QUERY) << "Matches selector" << selectors;
            result.prepend(rule);
        } else {
            qCDebug(UNION_QUERY) << "Does not match selectors" << selectors;
        }
    }

    std::stable_sort(result.begin(), result.end(), [](auto first, auto second) {
        return first->selectors().weight() > second->selectors().weight();
    });

    return result;
}

std::shared_ptr<Style> Style::create(const QString &pluginName, const QString &styleName, std::unique_ptr<StyleLoader> &&loader)
{
    auto d = std::make_unique<StylePrivate>();
    d->pluginName = pluginName;
    d->styleName = styleName;
    d->loader = std::move(loader);
    return std::make_shared<Style>(std::move(d));
}

bool Style::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == qApp && event->type() == QEvent::ApplicationPaletteChange) {
        StyleChangedEvent event;
        QCoreApplication::sendEvent(this, &event);
    }
    return QObject::eventFilter(obj, event);
}

StyleChangedEvent::StyleChangedEvent()
    : QEvent(s_type)
{
}
#include "moc_Style.cpp"
