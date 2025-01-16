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


#include <properties/AlignmentProperty.h>

// clang-format on

class QuickStyle;

class AlignmentPropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit AlignmentPropertyGroup(QuickStyle *style);

    void update(const Union::Properties::AlignmentProperty &newState);
    Q_SIGNAL void updated();

    Q_PROPERTY(QJSValue container READ container NOTIFY containerChanged)
    QJSValue container() const;
    Q_SIGNAL void containerChanged();

    Q_PROPERTY(QJSValue horizontal READ horizontal NOTIFY horizontalChanged)
    QJSValue horizontal() const;
    Q_SIGNAL void horizontalChanged();

    Q_PROPERTY(QJSValue vertical READ vertical NOTIFY verticalChanged)
    QJSValue vertical() const;
    Q_SIGNAL void verticalChanged();

    Q_PROPERTY(QJSValue order READ order NOTIFY orderChanged)
    QJSValue order() const;
    Q_SIGNAL void orderChanged();

private:
    QuickStyle *m_style = nullptr;
    Union::Properties::AlignmentProperty m_state;
};