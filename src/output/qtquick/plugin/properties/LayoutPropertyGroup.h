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


#include <properties/LayoutProperty.h>

#include "AlignmentPropertyGroup.h"
#include "SizePropertyGroup.h"
#include "SizePropertyGroup.h"
#include "SizePropertyGroup.h"
// clang-format on

class QuickStyle;

class LayoutPropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit LayoutPropertyGroup(QuickStyle *style);

    void update(const Union::Properties::LayoutProperty &newState);
    Q_SIGNAL void updated();

    Q_PROPERTY(AlignmentPropertyGroup *alignment READ alignment CONSTANT)
    AlignmentPropertyGroup *alignment() const;

    Q_PROPERTY(QJSValue width READ width NOTIFY widthChanged)
    QJSValue width() const;
    Q_SIGNAL void widthChanged();

    Q_PROPERTY(QJSValue height READ height NOTIFY heightChanged)
    QJSValue height() const;
    Q_SIGNAL void heightChanged();

    Q_PROPERTY(QJSValue spacing READ spacing NOTIFY spacingChanged)
    QJSValue spacing() const;
    Q_SIGNAL void spacingChanged();

    Q_PROPERTY(SizePropertyGroup *padding READ padding CONSTANT)
    SizePropertyGroup *padding() const;

    Q_PROPERTY(SizePropertyGroup *inset READ inset CONSTANT)
    SizePropertyGroup *inset() const;

    Q_PROPERTY(SizePropertyGroup *margins READ margins CONSTANT)
    SizePropertyGroup *margins() const;

private:
    QuickStyle *m_style = nullptr;
    std::unique_ptr<AlignmentPropertyGroup> m_alignment;
    std::unique_ptr<SizePropertyGroup> m_padding;
    std::unique_ptr<SizePropertyGroup> m_inset;
    std::unique_ptr<SizePropertyGroup> m_margins;
    Union::Properties::LayoutProperty m_state;
};