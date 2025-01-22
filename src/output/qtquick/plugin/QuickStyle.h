// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QEvent>
#include <QMarginsF>
#include <QObject>
#include <QProperty>
#include <QQuickAttachedPropertyPropagator>
#include <qqmlregistration.h>

#include <Element.h>
#include <ElementQuery.h>
#include <Selector.h>

#include "Types.h"
#include "properties/StylePropertyGroup.h"

class QuickElement;

/*!
 * \qmltype Style
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-core
 *
 * \brief An attached property that exposes style properties.
 *
 * This can be used to access style properties relevant to the current element
 * or child. It will try to find the closest QuickElement instance, either the
 * one attached to the same Item as this class is attached to, or to any parent
 * Item.
 */
class QuickStyle : public QQuickAttachedPropertyPropagator
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Style)
    QML_ATTACHED(QuickStyle)

public:
    QuickStyle(QQmlEngine *engine, QObject *parent = nullptr);

    /*!
     * \qmlattachedproperty StylePropertyGroup* Style::properties
     *
     * The properties that should be used to style the attached item.
     */
    Q_PROPERTY(StylePropertyGroup *properties READ properties CONSTANT)
    StylePropertyGroup *properties() const;

    /*!
     * The ElementQuery associated with this instance.
     *
     * This exposes the query from the QuickElement that this style is linked
     * to.
     */
    Union::ElementQuery *query() const;

    /*!
     * \qmlattachedsignal Style::updated
     *
     * Emitted whenever something in the underlying style rule selection changes.
     *
     * Most importantly, this will be emitted when the matched style rules
     * change due to a change in elements.
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

    static QuickStyle *qmlAttachedProperties(QObject *parent);

protected:
    void attachedParentChange(QQuickAttachedPropertyPropagator *, QQuickAttachedPropertyPropagator *) override;
    bool event(QEvent *event) override;

private:
    void setElement(QuickElement *newElement);
    void update();

    std::unique_ptr<StylePropertyGroup> m_properties;
    QPointer<QuickElement> m_element = nullptr;
    QQmlEngine *m_engine = nullptr;
};

class QuickStyleUpdatedEvent : public QEvent
{
public:
    QuickStyleUpdatedEvent();

    // Todo: Use registerEventType() instead of a hardcoded random offset
    inline static QEvent::Type s_type = QEvent::Type(QEvent::User + 57423);
};
