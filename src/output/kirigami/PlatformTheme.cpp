// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PlatformTheme.h"

#include <KIconColors>

#include "../qtquick/plugin/QuickStyle.h"

PlatformTheme::PlatformTheme(QObject *parent)
    : Kirigami::Platform::PlatformTheme(parent)
{
    setSupportsIconColoring(true);
    // TODO Find some way of maintaining inherit while using the correct colors.
    setInherit(false);
    syncColors();
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

    if (query->properties().background_or_new().color().has_value()) {
        setBackgroundColor(query->properties().background().value().color().value().toQColor());
    } else {
        setBackgroundColor(Qt::white);
    }

    if (query->properties().text_or_new().color().has_value()) {
        setTextColor(query->properties().text().value().color().value().toQColor());
        setHighlightedTextColor(query->properties().text().value().color().value().toQColor());
    } else {
        setTextColor(Qt::black);
    }

    // clang-format off
#define SET_IF_AVAILABLE(target, palette_value, default_value) \
    if (palette.has_value() && palette.value().palette_value().has_value()) { \
        target(palette.value().palette_value().value()); \
    } else { \
        target(default_value); \
    }
    // clang-format on
}

bool PlatformTheme::event(QEvent *event)
{
    if (event->type() == Kirigami::Platform::PlatformThemeEvents::DataChangedEvent::type) {
        syncColors();
    }

    if (event->type() == Kirigami::Platform::PlatformThemeEvents::ColorSetChangedEvent::type) {
        syncColors();
    }

    if (event->type() == Kirigami::Platform::PlatformThemeEvents::ColorGroupChangedEvent::type) {
        syncColors();
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
