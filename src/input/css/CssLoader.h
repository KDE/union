// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>

#include <Selector.h>
#include <StyleRule.h>

#include "ThemeLoader.h"

namespace cssparser
{
struct Property;
struct Selector;
struct SelectorPart;
}

using namespace Union;
using namespace Union::Properties;

class CssLoader : public ThemeLoader
{
public:
    bool load(std::shared_ptr<Theme> theme) override;

private:
    SelectorList createSelectorList(const cssparser::Selector &selector);
    Selector createSelector(const cssparser::SelectorPart &part);
    StyleProperty createProperties(const std::vector<cssparser::Property> &properties);

    void setLayoutProperty(StyleProperty &output, const cssparser::Property &property);
    void setBackgroundProperty(StyleProperty &output, const cssparser::Property &property);
    void setBorderProperty(StyleProperty &output, const cssparser::Property &property);
    void setOutlineProperty(StyleProperty &output, const cssparser::Property &property);
    void setTextProperty(StyleProperty &output, const cssparser::Property &property);
    void setIconProperty(StyleProperty &output, const cssparser::Property &property);
    void setShadowProperty(StyleProperty &output, const cssparser::Property &property);

    std::filesystem::path m_stylePath;
};
