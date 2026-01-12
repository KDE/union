// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "Style.h"

#include <filesystem>

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

#include "union_logging.h"
#include "union_query_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

UNION_EXPORT QEvent::Type StyleChangedEvent::s_type = QEvent::None;
static EventTypeRegistration<StyleChangedEvent> styleChangedEventRegistration;

class Union::StylePrivate
{
public:
    StylePrivate(const QString &_pluginName, const QString &_styleName, std::unique_ptr<StyleLoader> &&_loader)
        : pluginName(_pluginName)
        , styleName(_styleName)
        , loader(std::move(_loader))
    {
    }

    QString pluginName;
    QString styleName;

    std::unique_ptr<StyleLoader> loader;

    QList<StyleRule::Ptr> styles;
};

Style::Style(std::unique_ptr<StylePrivate> &&d)
    : QObject(nullptr)
    , d(std::move(d))
{
    qApp->installEventFilter(this);
}

Style::~Style() = default;

bool Style::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ApplicationPaletteChange) {
        StyleChangedEvent event;
        QCoreApplication::sendEvent(this, &event);
    }
    return QObject::eventFilter(obj, event);
}

QString Style::name() const
{
    return d->styleName;
}

QString Style::pluginName() const
{
    return d->pluginName;
}

StyleLoader *Style::loader() const
{
    return d->loader.get();
}

bool Style::load()
{
    Q_ASSERT_X(d->loader, "Union::Style", "Style requires a StyleLoader instance to function");
    return d->loader->load(shared_from_this());
}

void Style::insert(StyleRule::Ptr style)
{
    qCInfo(UNION_QUERY) << "Insert" << style;
    d->styles.append(style);
}

QList<StyleRule::Ptr> Style::rules()
{
    return d->styles;
}

QList<StyleRule::Ptr> Union::Style::matches(const QList<Element::Ptr> &elements)
{
    QList<StyleRule::Ptr> result;

    if (d->styles.isEmpty()) {
        qCInfo(UNION_QUERY) << "No style rules found for theme" << d->styleName << "so we will never match anything!";
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

    std::stable_sort(result.begin(), result.end(), [](auto first, auto second) {
        return first->selectors().weight() > second->selectors().weight();
    });

    return result;
}

std::shared_ptr<Style> Style::create(const QString &pluginName, const QString &styleName, std::unique_ptr<StyleLoader> &&loader)
{
    return std::make_shared<Style>(std::make_unique<StylePrivate>(pluginName, styleName, std::move(loader)));
}

StyleChangedEvent::StyleChangedEvent()
    : QEvent(s_type)
{
}