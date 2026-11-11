// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "TextSubElement.h"

#include <QPainter>

#include "UnionStyle.h"

using namespace Union::Widgets;
using namespace Union::Properties;

TextSubElement::TextSubElement(UnionStyle *style, QWidget *widget)
    : AbstractElementV2(style, widget)
{
}

void TextSubElement::setText(const QString &text)
{
    if (text == m_text) {
        return;
    }

    m_text = text;
    m_changed = true;
}

void TextSubElement::update(const QStyleOption *option)
{
    m_color = option->palette.text().color();
    if (option->palette.resolveMask() <= 0) {
        auto unionColor = safePropertyLookup(m_elementProperties, Union::Color{}, &StylePropertyGroup::text, &TextPropertyGroup::color);
        if (unionColor.isValid()) {
            m_color = unionColor.toQColor();
        }
    }

    m_enabled = option->state.testFlag(QStyle::State_Enabled);
    AbstractElementV2::update(option);
}

void TextSubElement::draw(QPainter *painter, [[maybe_unused]] DrawVariant what) const
{
    if (m_text.isEmpty()) {
        return;
    }

    int textFlags = Qt::AlignLeading | Qt::AlignVCenter;
    auto font = safePropertyLookup(m_elementProperties, painter->font(), &StylePropertyGroup::text, &TextPropertyGroup::font, &FontPropertyGroup::toQFont);
    textFlags = textFlagsFromProperties(m_elementProperties);
    if (m_flags.testFlag(AbstractElementFlag::DrawMnemonics)) {
        textFlags |= Qt::TextHideMnemonic;
    }

    painter->save();
    painter->setPen(m_color);
    painter->setFont(font);
    m_style->drawItemText(painter, m_rect.toRect(), textFlags, QPalette{}, m_enabled, m_text);
    painter->restore();
}
