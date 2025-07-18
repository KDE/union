// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
//
// This file is automatically generated from qml_group.cpp.j2.
// To regenerate, run `tools/propertygenerator/generate_properties.py`.

#include "ShadowPropertyGroup.h"

#include <QQmlEngine>

#include "QuickStyle.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

ShadowPropertyGroup::ShadowPropertyGroup(QuickStyle *style)
    : QObject()
    , m_style(style)
{
    m_offset = std::make_unique<OffsetPropertyGroup>(m_style);
    m_left = std::make_unique<LinePropertyGroup>(m_style);
    m_right = std::make_unique<LinePropertyGroup>(m_style);
    m_top = std::make_unique<LinePropertyGroup>(m_style);
    m_bottom = std::make_unique<LinePropertyGroup>(m_style);
    m_topLeft = std::make_unique<CornerPropertyGroup>(m_style);
    m_topRight = std::make_unique<CornerPropertyGroup>(m_style);
    m_bottomLeft = std::make_unique<CornerPropertyGroup>(m_style);
    m_bottomRight = std::make_unique<CornerPropertyGroup>(m_style);
}

void ShadowPropertyGroup::update(const ShadowProperty &newState)
{
    m_state = newState;
    m_offset->update(newState.offset().value_or(OffsetProperty{}));
    Q_EMIT colorChanged();
    Q_EMIT sizeChanged();
    Q_EMIT blurChanged();
    m_left->update(newState.left().value_or(LineProperty{}));
    m_right->update(newState.right().value_or(LineProperty{}));
    m_top->update(newState.top().value_or(LineProperty{}));
    m_bottom->update(newState.bottom().value_or(LineProperty{}));
    m_topLeft->update(newState.topLeft().value_or(CornerProperty{}));
    m_topRight->update(newState.topRight().value_or(CornerProperty{}));
    m_bottomLeft->update(newState.bottomLeft().value_or(CornerProperty{}));
    m_bottomRight->update(newState.bottomRight().value_or(CornerProperty{}));

    Q_EMIT updated();
}

OffsetPropertyGroup *ShadowPropertyGroup::offset() const
{
    return m_offset.get();
}

QJSValue ShadowPropertyGroup::color() const
{
    auto value = m_state.color();
    if (value) {
        return m_style->engine()->toScriptValue(value.value());
    }

    return QJSValue(QJSValue::UndefinedValue);
}

QJSValue ShadowPropertyGroup::size() const
{
    auto value = m_state.size();
    if (value) {
        return m_style->engine()->toScriptValue(value.value());
    }

    return QJSValue(QJSValue::UndefinedValue);
}

QJSValue ShadowPropertyGroup::blur() const
{
    auto value = m_state.blur();
    if (value) {
        return m_style->engine()->toScriptValue(value.value());
    }

    return QJSValue(QJSValue::UndefinedValue);
}

LinePropertyGroup *ShadowPropertyGroup::left() const
{
    return m_left.get();
}

LinePropertyGroup *ShadowPropertyGroup::right() const
{
    return m_right.get();
}

LinePropertyGroup *ShadowPropertyGroup::top() const
{
    return m_top.get();
}

LinePropertyGroup *ShadowPropertyGroup::bottom() const
{
    return m_bottom.get();
}

CornerPropertyGroup *ShadowPropertyGroup::topLeft() const
{
    return m_topLeft.get();
}

CornerPropertyGroup *ShadowPropertyGroup::topRight() const
{
    return m_topRight.get();
}

CornerPropertyGroup *ShadowPropertyGroup::bottomLeft() const
{
    return m_bottomLeft.get();
}

CornerPropertyGroup *ShadowPropertyGroup::bottomRight() const
{
    return m_bottomRight.get();
}

#include "moc_ShadowPropertyGroup.cpp"