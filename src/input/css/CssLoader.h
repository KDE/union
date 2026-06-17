// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>

#include <Selector.h>
#include <StyleRule.h>

#include "StyleLoader.h"

namespace cssparser
{
struct Property;
struct Selector;
struct SelectorPart;
}

using namespace Union;
using namespace Union::Properties;

class CssLoader : public StyleLoader
{
public:
    bool load(std::shared_ptr<Style> theme) override;

private:
    SelectorList createSelectorList(const cssparser::Selector &selector);
    Selector createSelector(const cssparser::SelectorPart &part);

    void createProperties(StylePropertyGroup *output, std::span<const cssparser::Property> properties);

    void setDisplayProperty(StylePropertyGroup *output, const cssparser::Property &property);
    void setLayoutProperty(StylePropertyGroup *output, const cssparser::Property &property);
    void setBackgroundProperty(StylePropertyGroup *output, const cssparser::Property &property);
    void setBorderProperty(StylePropertyGroup *output, const cssparser::Property &property);
    void setOutlineProperty(StylePropertyGroup *output, const cssparser::Property &property);
    void setTextProperty(StylePropertyGroup *output, const cssparser::Property &property);
    void setIconProperty(StylePropertyGroup *output, const cssparser::Property &property);
    void setShadowProperty(StylePropertyGroup *output, const cssparser::Property &property);

    std::filesystem::path m_stylePath;
};
