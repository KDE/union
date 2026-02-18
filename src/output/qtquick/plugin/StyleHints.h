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
     * \qmlattachedproperty bool StyleHints::showFramedBackground
     *
     * Whether to show a framed background or not.
     */
    Q_PROPERTY(bool showFramedBackground READ showFramedBackground WRITE setShowFramedBackground NOTIFY showFramedBackgroundChanged)
    bool showFramedBackground() const;
    void setShowFramedBackground(bool newShowFramedBackground);
    Q_SIGNAL void showFramedBackgroundChanged();

    /*!
     * \qmlattachedproperty int StyleHints::tickMarkStepSize
     *
     * The step size to use for tick marks. If > 0, will display tick marks.
     */
    Q_PROPERTY(int tickMarkStepSize READ tickMarkStepSize WRITE setTickMarkStepSize NOTIFY tickMarkStepSizeChanged)
    int tickMarkStepSize() const;
    void setTickMarkStepSize(int newTickMarkStepSize);
    Q_SIGNAL void tickMarkStepSizeChanged();

    /*!
     * \qmlattachedproperty string StyleHints::iconName
     *
     * The name of an icon theme icon.
     *
     * To be used for types that lack their own icon property.
     */
    Q_PROPERTY(QString iconName READ iconName WRITE setIconName NOTIFY iconNameChanged)
    QString iconName() const;
    void setIconName(const QString &newIconName);
    Q_SIGNAL void iconNameChanged();

    /*!
     * \qmlattachedproperty url StyleHints::iconSource
     *
     * The URL of an icon.
     *
     * To be used for types that lack their own icon property.
     */
    Q_PROPERTY(QUrl iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    QUrl iconSource() const;
    void setIconSource(const QUrl &newIconSource);
    Q_SIGNAL void iconSourceChanged();

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
    bool m_showFramedBackground = false;
    bool m_spellCheckEnabled = false;

    int m_tickMarkStepSize = 0;
    QString m_iconName;
    QUrl m_iconSource;
};

class StyleHintsChangedEvent : public QEvent
{
public:
    StyleHintsChangedEvent();

    static QEvent::Type s_type;
};
