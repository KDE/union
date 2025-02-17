// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from qml_group.cpp.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#include "AlignmentPropertyGroup.h"

#include <QQmlEngine>

#include "QuickStyle.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

AlignmentPropertyGroup::AlignmentPropertyGroup(QuickStyle *style)
    : QObject()
    , m_style(style)
{
}

void AlignmentPropertyGroup::update(const AlignmentProperty &newState)
{
    m_state = newState;
    Q_EMIT containerChanged();
    Q_EMIT horizontalChanged();
    Q_EMIT verticalChanged();
    Q_EMIT orderChanged();

    Q_EMIT updated();
}

QJSValue AlignmentPropertyGroup::container() const
{
    auto value = m_state.container();
    if (value) {
        return m_style->engine()->toScriptValue(value.value());
    }

    return QJSValue(QJSValue::UndefinedValue);
}

QJSValue AlignmentPropertyGroup::horizontal() const
{
    auto value = m_state.horizontal();
    if (value) {
        return m_style->engine()->toScriptValue(value.value());
    }

    return QJSValue(QJSValue::UndefinedValue);
}

QJSValue AlignmentPropertyGroup::vertical() const
{
    auto value = m_state.vertical();
    if (value) {
        return m_style->engine()->toScriptValue(value.value());
    }

    return QJSValue(QJSValue::UndefinedValue);
}

QJSValue AlignmentPropertyGroup::order() const
{
    auto value = m_state.order();
    if (value) {
        return m_style->engine()->toScriptValue(value.value());
    }

    return QJSValue(QJSValue::UndefinedValue);
}

#include "moc_AlignmentPropertyGroup.cpp"