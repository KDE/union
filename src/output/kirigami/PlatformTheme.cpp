// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PlatformTheme.h"

#include <KIconColors>

#include <Color.h>

#include "../qtquick/plugin/QuickStyle.h"

using namespace Qt::StringLiterals;
using namespace Union;

template<typename T>
inline QString enumToString(T value)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<T>();
    return QString::fromUtf8(metaEnum.valueToKey(value));
}

PlatformTheme::PlatformTheme(QObject *parent)
    : Kirigami::Platform::PlatformTheme(parent)
{
    setSupportsIconColoring(true);
    // TODO Find some way of maintaining inherit while using the correct colors.
    setInherit(false);
    syncColorSchemeColors();
}

PlatformTheme::~PlatformTheme()
{
}

QIcon PlatformTheme::iconFromTheme(const QString &name, const QColor &customColor)
{
    if (customColor != Qt::transparent) {
        KIconColors colors;
        colors.setText(customColor);
        return KDE::icon(name, colors);
    } else {
        return KDE::icon(name);
    }
}

void PlatformTheme::syncColors()
{
    if (!m_style) {
        m_style = static_cast<QuickStyle *>(qmlAttachedPropertiesObject<QuickStyle>(parent()));
        if (!m_style) {
            return;
        }
        m_style->installEventFilter(this);
    }

    auto query = m_style->query();
    if (!query || !query->hasMatches()) {
        return;
    }

    Kirigami::Platform::PlatformThemeChangeTracker tracker(this);

    if (query->properties().background_or_new().color().has_value()) {
        setBackgroundColor(query->properties().background().value().color().value().toQColor());
    }

    if (query->properties().text_or_new().color().has_value()) {
        setTextColor(query->properties().text().value().color().value().toQColor());
    }
}

bool PlatformTheme::event(QEvent *event)
{
    if (event->type() == Kirigami::Platform::PlatformThemeEvents::DataChangedEvent::type) {
        syncColorSchemeColors();
    }

    if (event->type() == Kirigami::Platform::PlatformThemeEvents::ColorSetChangedEvent::type) {
        syncColorSchemeColors();
    }

    if (event->type() == Kirigami::Platform::PlatformThemeEvents::ColorGroupChangedEvent::type) {
        syncColorSchemeColors();
    }

    return Kirigami::Platform::PlatformTheme::event(event);
}

bool PlatformTheme::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QuickStyleUpdatedEvent::s_type) {
        syncColors();
        return false;
    }

    return Kirigami::Platform::PlatformTheme::eventFilter(target, event);
}

void PlatformTheme::syncColorSchemeColors()
{
    // Temporary workaround to make sure our PlatformTheme is half-decently
    // usable with Kirigami until we figure out a good way to provide these
    // values from the input layer.

    if (QCoreApplication::closingDown()) {
        return;
    }

    QString group = enumToString<PlatformTheme::ColorGroup>(colorGroup());

    auto parentItem = qobject_cast<QQuickItem *>(parent());
    if (parentItem) {
        if (!parentItem->isEnabled()) {
            group = u"disabled"_s;
            // } else if (m_window && !m_window->isActive() && m_window->isExposed()) {
            //     // Why also checking the window is exposed?
            //     // in the case of QQuickWidget the window() will never be active
            //     // and the widgets will always have the inactive palette.
            //     // better to always show it active than always show it inactive
            //     group = u"inactive"_s;
        }
    }

    Kirigami::Platform::PlatformThemeChangeTracker tracker(this);

    QString set = enumToString<PlatformTheme::ColorSet>(colorSet());

    // foreground
    setTextColor(Color::custom(u"kcolorscheme"_s, {group, set, u"foreground"_s, u"text"_s}).toQColor());
    setDisabledTextColor(Color::custom(u"kcolorscheme"_s, {group, set, u"foreground"_s, u"inactive"_s}).toQColor());
    setHighlightedTextColor(Color::custom(u"kcolorscheme"_s, {group, u"selection"_s, u"foreground"_s, u"text"_s}).toQColor());
    setActiveTextColor(Color::custom(u"kcolorscheme"_s, {group, set, u"foreground"_s, u"active"_s}).toQColor());

    setLinkColor(Color::custom(u"kcolorscheme"_s, {group, set, u"foreground"_s, u"link"_s}).toQColor());
    setVisitedLinkColor(Color::custom(u"kcolorscheme"_s, {group, set, u"foreground"_s, u"visited"_s}).toQColor());

    setPositiveTextColor(Color::custom(u"kcolorscheme"_s, {group, set, u"foreground"_s, u"positive"_s}).toQColor());
    setNeutralTextColor(Color::custom(u"kcolorscheme"_s, {group, set, u"foreground"_s, u"neutral"_s}).toQColor());
    setNegativeTextColor(Color::custom(u"kcolorscheme"_s, {group, set, u"foreground"_s, u"negative"_s}).toQColor());

    // background
    setHighlightColor(Color::custom(u"kcolorscheme"_s, {group, u"selection"_s, u"background"_s, u"normal"_s}).toQColor());
    setBackgroundColor(Color::custom(u"kcolorscheme"_s, {group, set, u"background"_s, u"normal"_s}).toQColor());
    setAlternateBackgroundColor(Color::custom(u"kcolorscheme"_s, {group, set, u"background"_s, u"alternate"_s}).toQColor());
    setActiveBackgroundColor(Color::custom(u"kcolorscheme"_s, {group, set, u"background"_s, u"active"_s}).toQColor());

    setLinkBackgroundColor(Color::custom(u"kcolorscheme"_s, {group, set, u"background"_s, u"link"_s}).toQColor());
    setVisitedLinkBackgroundColor(Color::custom(u"kcolorscheme"_s, {group, set, u"background"_s, u"visited"_s}).toQColor());

    setPositiveBackgroundColor(Color::custom(u"kcolorscheme"_s, {group, set, u"background"_s, u"positive"_s}).toQColor());
    setNeutralBackgroundColor(Color::custom(u"kcolorscheme"_s, {group, set, u"background"_s, u"neutral"_s}).toQColor());
    setNegativeBackgroundColor(Color::custom(u"kcolorscheme"_s, {group, set, u"background"_s, u"negative"_s}).toQColor());

    // decoration
    setHoverColor(Color::custom(u"kcolorscheme"_s, {group, set, u"decoration"_s, u"hover"_s}).toQColor());
    setFocusColor(Color::custom(u"kcolorscheme"_s, {group, set, u"decoration"_s, u"focus"_s}).toQColor());

    syncColors();
}
