// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "TextSubElement.h"

using namespace Union::Widgets;
using namespace Union::Properties;

TextSubElement::TextSubElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
    setFlag(AbstractElementFlag::SubElement);
    setFlag(AbstractElementFlag::DrawMnemonics);
}

QString TextSubElement::text() const
{
    return m_text;
}

void TextSubElement::setText(const QString &text)
{
    if (text == m_text) {
        return;
    }

    m_text = text;
    markDirty();
}

void TextSubElement::update(const QStyleOption *option)
{
    m_color = option->palette.text().color();
    if (option->palette.resolveMask() <= 0) {
        auto unionColor = safePropertyLookup(properties(), Union::Color{}, &StylePropertyGroup::text, &TextPropertyGroup::color);
        if (unionColor.isValid()) {
            m_color = unionColor.toQColor();
        }
    }

    m_enabled = option->state.testFlag(QStyle::State_Enabled);
    m_direction = option->direction;

    AbstractElement::update(option);
}

void TextSubElement::draw(QPainter *painter, [[maybe_unused]] DrawVariant what) const
{
    if (m_text.isEmpty()) {
        return;
    }

    int textFlags = Qt::AlignLeading | Qt::AlignVCenter;
    auto font = safePropertyLookup(properties(), painter->font(), &StylePropertyGroup::text, &TextPropertyGroup::font, &FontPropertyGroup::toQFont);
    textFlags = propertyWrapper().textFlags(m_direction);
    if (isFlagSet(AbstractElementFlag::DrawMnemonics)) {
        textFlags |= Qt::TextHideMnemonic;
    }

    painter->save();
    painter->setPen(m_color);
    painter->setFont(font);
    style()->drawItemText(painter, rect().toRect(), textFlags, QPalette{}, m_enabled, m_text);
    painter->restore();
}
