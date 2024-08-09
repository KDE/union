// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `generate_properties.py`.

#include "StylePropertyGroup.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

StylePropertyGroup::StylePropertyGroup()
{
    m_palette = std::make_unique<PalettePropertyGroup>();
    m_layout = std::make_unique<LayoutPropertyGroup>();
    m_text = std::make_unique<TextPropertyGroup>();
    m_icon = std::make_unique<IconPropertyGroup>();
    m_background = std::make_unique<BackgroundPropertyGroup>();
}

void StylePropertyGroup::update(const StyleProperty &newState)
{
    m_palette->update(newState.palette().value_or(PaletteProperty{}));
    m_layout->update(newState.layout().value_or(LayoutProperty{}));
    m_text->update(newState.text().value_or(TextProperty{}));
    m_icon->update(newState.icon().value_or(IconProperty{}));
    m_background->update(newState.background().value_or(BackgroundProperty{}));
}

PalettePropertyGroup *StylePropertyGroup::palette() const
{
    return m_palette.get();
}

LayoutPropertyGroup *StylePropertyGroup::layout() const
{
    return m_layout.get();
}

TextPropertyGroup *StylePropertyGroup::text() const
{
    return m_text.get();
}

IconPropertyGroup *StylePropertyGroup::icon() const
{
    return m_icon.get();
}

BackgroundPropertyGroup *StylePropertyGroup::background() const
{
    return m_background.get();
}

#include "moc_StylePropertyGroup.cpp"