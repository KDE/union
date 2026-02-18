// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
// SPDX-FileCopyrightText: 2025 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include <QEvent>
#include <QObject>
#include <QQmlEngine>
#include <qqmlregistration.h>

#include "properties/StylePropertyGroup.h"

#include "unionquickimpl_export.h"

class QuickElement;

/*!
 * \qmltype StyleHints
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-core
 *
 * \brief An attached property with hints.
 *
 */
class UNIONQUICKIMPL_EXPORT StyleHints : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Attached Property")
    QML_ATTACHED(StyleHints)

public:
    explicit StyleHints(QObject *parent = nullptr);

    /*!
     * \qmlattachedproperty bool StyleHints::useAlternatingColors
     *
     * Provide backwards compatibility for `Kirigami.Theme.useAlternateBackgroundColor`.
     */
    Q_PROPERTY(bool useAlternatingColors READ useAlternatingColors WRITE setUseAlternatingColors NOTIFY useAlternatingColorsChanged)
    bool useAlternatingColors() const;
    void setUseAlternatingColors(bool newUseAlternatingColors);
    Q_SIGNAL void useAlternatingColorsChanged();

    /*!
     * \qmlattachedproperty bool StyleHints::SpellCheckEnabled
     *
     * TODO: Provide backwards compatibility for `Kirigami.SpellCheck.enabled`.
     * NOTE: Currently this is just a stub!
     */
    Q_PROPERTY(bool spellCheckEnabled READ spellCheckEnabled WRITE setSpellCheckEnabled NOTIFY spellCheckEnabledChanged)
    bool spellCheckEnabled() const;
    void setSpellCheckEnabled(const bool &enabled);
    Q_SIGNAL void spellCheckEnabledChanged();

    /*!
     * \qmlattachedsignal StyleHints::updated
     *
     * Emitted whenever something in StyleHints changes.
     */
    Q_SIGNAL void updated();

    static StyleHints *qmlAttachedProperties(QObject *parent);

private:
    void update();

    bool m_useAlternatingColors = false;
};

class StyleHintsChangedEvent : public QEvent
{
public:
    StyleHintsChangedEvent();

    static QEvent::Type s_type;
};
