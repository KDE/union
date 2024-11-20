// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off
#include <QObject>
#include <QProperty>
#include <qqmlregistration.h>


#include <properties/TextProperty.h>

#include "AlignmentPropertyGroup.h"
// clang-format on

class TextPropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    TextPropertyGroup();

    void update(const Union::Properties::TextProperty &newState);
    Q_SIGNAL void updated();

    Q_PROPERTY(AlignmentPropertyGroup *alignment READ alignment CONSTANT)
    AlignmentPropertyGroup *alignment() const;

    Q_PROPERTY(QFont font READ font NOTIFY fontChanged)
    QFont font() const;
    Q_SIGNAL void fontChanged();

private:
    std::unique_ptr<AlignmentPropertyGroup> m_alignment;
    Union::Properties::TextProperty m_state;
};