// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

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

class LinePropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit LinePropertyGroup(QuickStyle *style);

    void update(const Union::Properties::LineProperty &newState);
    Q_SIGNAL void updated();

    Q_PROPERTY(QJSValue size READ size NOTIFY sizeChanged)
    QJSValue size() const;
    Q_SIGNAL void sizeChanged();

    Q_PROPERTY(QJSValue color READ color NOTIFY colorChanged)
    QJSValue color() const;
    Q_SIGNAL void colorChanged();

    Q_PROPERTY(QJSValue style READ style NOTIFY styleChanged)
    QJSValue style() const;
    Q_SIGNAL void styleChanged();

    Q_PROPERTY(ImagePropertyGroup *image READ image CONSTANT)
    ImagePropertyGroup *image() const;

private:
    QuickStyle *m_style = nullptr;
    std::unique_ptr<ImagePropertyGroup> m_image;
    Union::Properties::LineProperty m_state;
};