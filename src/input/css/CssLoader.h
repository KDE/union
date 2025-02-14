// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <Selector.h>
#include <StyleRule.h>

#include "ThemeLoader.h"

namespace cssparser
{
struct Property;
struct Selector;
struct SelectorPart;
}

class CssLoader : public Union::ThemeLoader
{
public:
    bool load(std::shared_ptr<Union::Theme> theme) override;

private:
    Union::SelectorList createSelectorList(const cssparser::Selector &selector);
    Union::Selector createSelector(const cssparser::SelectorPart &part);
    Union::Properties::StyleProperty createProperties(const std::vector<cssparser::Property> &properties);

    void setLayoutProperty(Union::Properties::StyleProperty &output, const cssparser::Property &property);
    void setBackgroundProperty(Union::Properties::StyleProperty &output, const cssparser::Property &property);
    void setBorderProperty(Union::Properties::StyleProperty &output, const cssparser::Property &property);
};
