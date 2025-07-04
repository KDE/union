// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from qml_group.cpp.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#include "OutlinePropertyGroup.h"

#include <QQmlEngine>

#include "QuickStyle.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

OutlinePropertyGroup::OutlinePropertyGroup(QuickStyle *style)
    : QObject()
    , m_style(style)
{
    m_left = std::make_unique<LinePropertyGroup>(m_style);
    m_right = std::make_unique<LinePropertyGroup>(m_style);
    m_top = std::make_unique<LinePropertyGroup>(m_style);
    m_bottom = std::make_unique<LinePropertyGroup>(m_style);
}

void OutlinePropertyGroup::update(const OutlineProperty &newState)
{
    m_state = newState;
    m_left->update(newState.left().value_or(LineProperty{}));
    m_right->update(newState.right().value_or(LineProperty{}));
    m_top->update(newState.top().value_or(LineProperty{}));
    m_bottom->update(newState.bottom().value_or(LineProperty{}));

    Q_EMIT updated();
}

LinePropertyGroup *OutlinePropertyGroup::left() const
{
    return m_left.get();
}

LinePropertyGroup *OutlinePropertyGroup::right() const
{
    return m_right.get();
}

LinePropertyGroup *OutlinePropertyGroup::top() const
{
    return m_top.get();
}

LinePropertyGroup *OutlinePropertyGroup::bottom() const
{
    return m_bottom.get();
}

#include "moc_OutlinePropertyGroup.cpp"