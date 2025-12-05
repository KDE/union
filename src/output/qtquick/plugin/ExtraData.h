// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QEvent>
#include <QObject>
#include <QQmlEngine>
#include <QQmlParserStatus>
#include <QQuickAttachedPropertyPropagator>
#include <qqmlregistration.h>

#include "properties/StylePropertyGroup.h"

#include "unionquickimpl_export.h"

class QuickElement;

/*!
 * \qmltype ExtraData
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-core
 *
 * \brief An attached property that exposes extra data properties.
 *
 */
class UNIONQUICKIMPL_EXPORT ExtraData : public QQuickAttachedPropertyPropagator, public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ExtraData)
    QML_UNCREATABLE("Attached Property")
    QML_ATTACHED(ExtraData)
    Q_INTERFACES(QQmlParserStatus)

public:
    ExtraData(QQmlEngine *engine, QObject *parent = nullptr);

    /*!
     * \qmlattachedproperty QVariantMap ExtraData::data
     * A map of extra data that can be used to get data outside of Union for styling.
     *
     * Current most prominent example for this is providing backwards compatibility
     * for `Kirigami.Theme.useAlternateBackgroundColor`.
     */
    Q_PROPERTY(QVariantMap data READ data WRITE setData NOTIFY dataChanged)
    QVariantMap data() const;
    void setData(const QVariantMap &newData);
    Q_SIGNAL void dataChanged();

    /*!
     * \qmlattachedsignal ExtraData::updated
     *
     * Emitted whenever something in the ExtraData data `QVariantMap` cahnges.
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

    static ExtraData *qmlAttachedProperties(QObject *parent);

protected:
    void attachedParentChange(QQuickAttachedPropertyPropagator *, QQuickAttachedPropertyPropagator *) override;
    bool event(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void classBegin() override;
    void componentComplete() override;

private:
    void update();

    std::unique_ptr<StylePropertyGroup> m_properties;
    QVariantMap m_extraData;
    QQmlEngine *m_engine = nullptr;

    bool m_completed = false;
};

class ExtraDataUpdatedEvent : public QEvent
{
public:
    ExtraDataUpdatedEvent();

    inline static QEvent::Type s_type = QEvent::None;
};
