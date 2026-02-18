// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

#include "StyleHints.h"

#include <QCoreApplication>
#include <QQmlEngine>

#include <Element.h>
#include <EventHelper.h>
#include <StyleRule.h>

using namespace Union;

QEvent::Type StyleHintsChangedEvent::s_type = QEvent::None;
static EventTypeRegistration<StyleHintsChangedEvent> styleHintsRegistration;

StyleHints::StyleHints(QObject *parent)
    : QObject(parent)
{
    update();
}

bool StyleHints::useAlternatingColors() const
{
    return m_useAlternatingColors;
}

void StyleHints::setUseAlternatingColors(bool newUseAlternatingColors)
{
    if (m_useAlternatingColors != newUseAlternatingColors) {
        m_useAlternatingColors = newUseAlternatingColors;
        update();
        Q_EMIT useAlternatingColorsChanged();
    }
}

bool StyleHints::spellCheckEnabled() const
{
    return m_spellCheckEnabled;
}

void StyleHints::setSpellCheckEnabled(const bool &enabled)
{
    if (m_spellCheckEnabled != enabled) {
        m_spellCheckEnabled = enabled;
        update();
        Q_EMIT spellCheckEnabledChanged();
    }
}

StyleHints *StyleHints::qmlAttachedProperties(QObject *parent)
{
    return new StyleHints(parent);
}

void StyleHints::update()
{
    StyleHintsChangedEvent event;
    QCoreApplication::sendEvent(this, &event);

    Q_EMIT updated();
}

StyleHintsChangedEvent::StyleHintsChangedEvent()
    : QEvent(s_type)
{
}
