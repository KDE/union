/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

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

/**
 * An attached property that exposes style properties.
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
    QuickStyle(QObject *parent = nullptr);

    Q_PROPERTY(StylePropertyGroup *properties READ properties CONSTANT)
    StylePropertyGroup *properties() const;

    /**
     * The ElementQuery associated with this instance.
     *
     * This exposes the query from the QuickElement that this style is linked
     * to.
     */
    Union::ElementQuery *query() const;

    /**
     * Emitted whenever something in the underlying style rule selection changes.
     *
     * Most importantly, this will be emitted when the matched style rules
     * change due to a change in elements.
     */
    Q_SIGNAL void updated();

    static QuickStyle *qmlAttachedProperties(QObject *parent);

protected:
    void attachedParentChange(QQuickAttachedPropertyPropagator *, QQuickAttachedPropertyPropagator *) override;

private:
    void setElement(QuickElement *newElement);
    void update();

    std::unique_ptr<StylePropertyGroup> m_properties;

    QuickElement *m_element = nullptr;
};
