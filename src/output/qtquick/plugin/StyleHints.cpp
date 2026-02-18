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

bool StyleHints::showFramedBackground() const
{
    return m_showFramedBackground;
}

void StyleHints::setShowFramedBackground(bool newShowFramedBackground)
{
    if (newShowFramedBackground == m_showFramedBackground) {
        return;
    }

    m_showFramedBackground = newShowFramedBackground;
    update();
    Q_EMIT showFramedBackgroundChanged();
}

int StyleHints::tickMarkStepSize() const
{
    return m_tickMarkStepSize;
}

void StyleHints::setTickMarkStepSize(int newTickMarkStepSize)
{
    if (newTickMarkStepSize == m_tickMarkStepSize) {
        return;
    }

    m_tickMarkStepSize = newTickMarkStepSize;
    update();
    Q_EMIT tickMarkStepSizeChanged();
}

QString StyleHints::iconName() const
{
    return m_iconName;
}

void StyleHints::setIconName(const QString &newIconName)
{
    if (newIconName == m_iconName) {
        return;
    }

    m_iconName = newIconName;
    update();
    Q_EMIT iconNameChanged();
}

QUrl StyleHints::iconSource() const
{
    return m_iconSource;
}

void StyleHints::setIconSource(const QUrl &newIconSource)
{
    if (newIconSource == m_iconSource) {
        return;
    }

    m_iconSource = newIconSource;
    update();
    Q_EMIT iconSourceChanged();
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
