// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off
#include <QObject>
#include <QProperty>
#include <qqmlregistration.h>


#include <properties/AlignmentProperty.h>

// clang-format on

class AlignmentPropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    AlignmentPropertyGroup();

    void update(const Union::Properties::AlignmentProperty &newState);
    Q_SIGNAL void updated();

    Q_PROPERTY(Union::Properties::AlignmentContainer container READ container NOTIFY containerChanged)
    Union::Properties::AlignmentContainer container() const;
    Q_SIGNAL void containerChanged();

    Q_PROPERTY(Union::Properties::Alignment horizontal READ horizontal NOTIFY horizontalChanged)
    Union::Properties::Alignment horizontal() const;
    Q_SIGNAL void horizontalChanged();

    Q_PROPERTY(Union::Properties::Alignment vertical READ vertical NOTIFY verticalChanged)
    Union::Properties::Alignment vertical() const;
    Q_SIGNAL void verticalChanged();

    Q_PROPERTY(int order READ order NOTIFY orderChanged)
    int order() const;
    Q_SIGNAL void orderChanged();

private:
    Union::Properties::AlignmentProperty m_state;
};