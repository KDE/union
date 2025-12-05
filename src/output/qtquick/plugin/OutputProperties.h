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
 * \qmltype OutputProperties
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-core
 *
 * \brief An attached property that exposes properties from output.
 *
 */
class UNIONQUICKIMPL_EXPORT OutputProperties : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(OutputProperties)
    QML_UNCREATABLE("Attached Property")
    QML_ATTACHED(OutputProperties)

public:
    OutputProperties(QQmlEngine *engine, QObject *parent = nullptr);

    /*!
     * \qmlattachedproperty bool OutputProperties::useAlternatingColors
     *
     * Provide backwards compatibility for `Kirigami.Theme.useAlternateBackgroundColor`.
     */
    Q_PROPERTY(bool useAlternatingColors READ useAlternatingColors WRITE setUseAlternatingColors NOTIFY useAlternatingColorsChanged)
    bool useAlternatingColors() const;
    void setUseAlternatingColors(bool newUseAlternatingColors);
    Q_SIGNAL void useAlternatingColorsChanged();

    /*!
     * \qmlattachedsignal OutputProperties::updated
     *
     * Emitted whenever something in OutputProperties changes.
     */
    Q_SIGNAL void updated();
    /**
     * The QML engine associated with this instance.
     *
     * Unfortunately, due to some internal workings of the QML engine,
     * `qmlEngine()` of an attached property returns nullptr. So instead, we
     * have to manually handle it.
     */
    QQmlEngine *engine() const;

    static OutputProperties *qmlAttachedProperties(QObject *parent);

private:
    void update();

    std::unique_ptr<StylePropertyGroup> m_properties;
    bool m_useAlternatingColors = false;
    QQmlEngine *m_engine = nullptr;

    bool m_completed = false;
};

class OutputPropertiesUpdatedEvent : public QEvent
{
public:
    OutputPropertiesUpdatedEvent();

    inline static QEvent::Type s_type = QEvent::None;
};
