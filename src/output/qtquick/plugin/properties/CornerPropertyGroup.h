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


#include <properties/CornerProperty.h>

#include "ImagePropertyGroup.h"
// clang-format on

class QuickStyle;

/*!
 * \qmltype CornerPropertyGroup
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-properties
 *
 * \brief A group object exposing the properties of CornerProperty to QML.
 */
class CornerPropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit CornerPropertyGroup(QuickStyle *style);

    void update(const Union::Properties::CornerProperty &newState);
    Q_SIGNAL void updated();

    /*!
     * \qmlproperty qreal CornerPropertyGroup::radius
     *
     * Exposes CornerProperty::radius to QML.
     */
    Q_PROPERTY(QJSValue radius READ radius NOTIFY radiusChanged)
    QJSValue radius() const;
    Q_SIGNAL void radiusChanged();

    /*!
     * \qmlproperty qreal CornerPropertyGroup::width
     *
     * Exposes CornerProperty::width to QML.
     */
    Q_PROPERTY(QJSValue width READ width NOTIFY widthChanged)
    QJSValue width() const;
    Q_SIGNAL void widthChanged();

    /*!
     * \qmlproperty qreal CornerPropertyGroup::height
     *
     * Exposes CornerProperty::height to QML.
     */
    Q_PROPERTY(QJSValue height READ height NOTIFY heightChanged)
    QJSValue height() const;
    Q_SIGNAL void heightChanged();

    /*!
     * \qmlproperty QColor CornerPropertyGroup::color
     *
     * Exposes CornerProperty::color to QML.
     */
    Q_PROPERTY(QJSValue color READ color NOTIFY colorChanged)
    QJSValue color() const;
    Q_SIGNAL void colorChanged();

    /*!
     * \qmlproperty ImagePropertyGroup CornerPropertyGroup::image
     *
     * Exposes CornerProperty::ImageProperty to QML.
     */
    Q_PROPERTY(ImagePropertyGroup *image READ image CONSTANT)
    ImagePropertyGroup *image() const;

private:
    QuickStyle *m_style = nullptr;
    std::unique_ptr<ImagePropertyGroup> m_image;
    Union::Properties::CornerProperty m_state;
};