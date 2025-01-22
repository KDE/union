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


#include <properties/LineProperty.h>

#include "ImagePropertyGroup.h"
// clang-format on

class QuickStyle;

/*!
 * \qmltype LinePropertyGroup
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-properties
 *
 * \brief A group object exposing the properties of LineProperty to QML.
 */
class LinePropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit LinePropertyGroup(QuickStyle *style);

    void update(const Union::Properties::LineProperty &newState);
    Q_SIGNAL void updated();

    /*!
     * \qmlproperty qreal LinePropertyGroup::size
     *
     * Exposes LineProperty::size to QML.
     */
    Q_PROPERTY(QJSValue size READ size NOTIFY sizeChanged)
    QJSValue size() const;
    Q_SIGNAL void sizeChanged();

    /*!
     * \qmlproperty QColor LinePropertyGroup::color
     *
     * Exposes LineProperty::color to QML.
     */
    Q_PROPERTY(QJSValue color READ color NOTIFY colorChanged)
    QJSValue color() const;
    Q_SIGNAL void colorChanged();

    /*!
     * \qmlproperty Union::Properties::LineStyle LinePropertyGroup::style
     *
     * Exposes LineProperty::style to QML.
     */
    Q_PROPERTY(QJSValue style READ style NOTIFY styleChanged)
    QJSValue style() const;
    Q_SIGNAL void styleChanged();

    /*!
     * \qmlproperty ImagePropertyGroup LinePropertyGroup::image
     *
     * Exposes LineProperty::ImageProperty to QML.
     */
    Q_PROPERTY(ImagePropertyGroup *image READ image CONSTANT)
    ImagePropertyGroup *image() const;

private:
    QuickStyle *m_style = nullptr;
    std::unique_ptr<ImagePropertyGroup> m_image;
    Union::Properties::LineProperty m_state;
};