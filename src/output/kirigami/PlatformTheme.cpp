// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "PlatformTheme.h"

#include <KIconColors>

#include "../qtquick/plugin/QuickStyle.h"

PlatformTheme::PlatformTheme(QObject *parent)
    : Kirigami::Platform::PlatformTheme(parent)
{
    setSupportsIconColoring(true);
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
        auto parentItem = qobject_cast<QQuickItem *>(parent());
        m_style = static_cast<QuickStyle *>(qmlAttachedPropertiesObject<QuickStyle>(parentItem));
        if (!m_style) {
            return;
        }
        connect(m_style, &QuickStyle::updated, this, &PlatformTheme::syncColors);
    }

    auto query = m_style->query();
    if (!query || !query->hasMatches()) {
        return;
    }

    auto palette = query->properties().palette();

    if (query->properties().background_or_new().color().has_value()) {
        setBackgroundColor(query->properties().background().value().color().value());
    } else if (palette.has_value() && palette.value().window().has_value()) {
        setBackgroundColor(palette.value().window().value());
    } else {
        setBackgroundColor(Qt::white);
    }

    if (query->properties().text_or_new().color().has_value()) {
        setTextColor(query->properties().text().value().color().value());
        setHighlightedTextColor(query->properties().text().value().color().value());
    } else if (palette.has_value() && palette.value().windowText().has_value()) {
        setTextColor(palette.value().windowText().value());
        setHighlightedTextColor(palette.value().windowText().value());
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
