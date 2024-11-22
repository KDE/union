// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off
#include <QJSValue>
#include <QObject>
#include <QProperty>
#include <qqmlregistration.h>


#include <properties/TextProperty.h>

#include "AlignmentPropertyGroup.h"
// clang-format on

class QuickStyle;

class TextPropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit TextPropertyGroup(QuickStyle *style);

    void update(const Union::Properties::TextProperty &newState);
    Q_SIGNAL void updated();

    Q_PROPERTY(AlignmentPropertyGroup *alignment READ alignment CONSTANT)
    AlignmentPropertyGroup *alignment() const;

    Q_PROPERTY(QJSValue font READ font NOTIFY fontChanged)
    QJSValue font() const;
    Q_SIGNAL void fontChanged();

private:
    QuickStyle *m_style = nullptr;
    std::unique_ptr<AlignmentPropertyGroup> m_alignment;
    Union::Properties::TextProperty m_state;
};