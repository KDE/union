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

    // clang-format off
#define SET_IF_AVAILABLE(target, palette_value, default_value) \
    if (palette.has_value() && palette.value().palette_value().has_value()) { \
        target(palette.value().palette_value().value()); \
    } else { \
        target(default_value); \
    }
    // clang-format on

    SET_IF_AVAILABLE(setBackgroundColor, window, Qt::white);
    SET_IF_AVAILABLE(setTextColor, windowText, Qt::black);
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
