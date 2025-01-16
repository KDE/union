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


#include <properties/BorderProperty.h>

#include "LinePropertyGroup.h"
#include "LinePropertyGroup.h"
#include "LinePropertyGroup.h"
#include "LinePropertyGroup.h"
// clang-format on

class QuickStyle;

class BorderPropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit BorderPropertyGroup(QuickStyle *style);

    void update(const Union::Properties::BorderProperty &newState);
    Q_SIGNAL void updated();

    Q_PROPERTY(LinePropertyGroup *left READ left CONSTANT)
    LinePropertyGroup *left() const;

    Q_PROPERTY(LinePropertyGroup *right READ right CONSTANT)
    LinePropertyGroup *right() const;

    Q_PROPERTY(LinePropertyGroup *top READ top CONSTANT)
    LinePropertyGroup *top() const;

    Q_PROPERTY(LinePropertyGroup *bottom READ bottom CONSTANT)
    LinePropertyGroup *bottom() const;

private:
    QuickStyle *m_style = nullptr;
    std::unique_ptr<LinePropertyGroup> m_left;
    std::unique_ptr<LinePropertyGroup> m_right;
    std::unique_ptr<LinePropertyGroup> m_top;
    std::unique_ptr<LinePropertyGroup> m_bottom;
    Union::Properties::BorderProperty m_state;
};