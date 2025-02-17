// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from qml_group.h.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off
#include <QJSValue>
#include <QObject>
#include <QProperty>
#include <qqmlregistration.h>


#include <properties/SizeProperty.h>

// clang-format on

class QuickStyle;

/*!
 * \qmltype SizePropertyGroup
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-properties
 *
 * \brief A group object exposing the properties of SizeProperty to QML.
 */
class SizePropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit SizePropertyGroup(QuickStyle *style);

    void update(const Union::Properties::SizeProperty &newState);
    Q_SIGNAL void updated();

    /*!
     * \qmlproperty qreal SizePropertyGroup::left
     *
     * Exposes SizeProperty::left to QML.
     */
    Q_PROPERTY(QJSValue left READ left NOTIFY leftChanged)
    QJSValue left() const;
    Q_SIGNAL void leftChanged();

    /*!
     * \qmlproperty qreal SizePropertyGroup::right
     *
     * Exposes SizeProperty::right to QML.
     */
    Q_PROPERTY(QJSValue right READ right NOTIFY rightChanged)
    QJSValue right() const;
    Q_SIGNAL void rightChanged();

    /*!
     * \qmlproperty qreal SizePropertyGroup::top
     *
     * Exposes SizeProperty::top to QML.
     */
    Q_PROPERTY(QJSValue top READ top NOTIFY topChanged)
    QJSValue top() const;
    Q_SIGNAL void topChanged();

    /*!
     * \qmlproperty qreal SizePropertyGroup::bottom
     *
     * Exposes SizeProperty::bottom to QML.
     */
    Q_PROPERTY(QJSValue bottom READ bottom NOTIFY bottomChanged)
    QJSValue bottom() const;
    Q_SIGNAL void bottomChanged();

private:
    QuickStyle *m_style = nullptr;
    Union::Properties::SizeProperty m_state;
};