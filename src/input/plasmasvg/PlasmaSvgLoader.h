/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QMetaEnum>
#include <QPalette>

#include <Plasma/Theme>

#define RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS
#include <ryml.hpp>
#include <ryml_std.hpp>

#include <Element.h>
#include <StyleRule.h>

#include "ThemeLoader.h"

class QSvgRenderer;
class PlasmaSvgRenderer;

namespace Union
{
struct CornerDefinition;
struct LineDefinition;
struct ImageDefinition;
}

struct LoadingContext;

class PlasmaSvgLoader : public Union::ThemeLoader
{
public:
    bool load(std::shared_ptr<Union::Theme> theme) override;

    const Plasma::Theme *plasmaTheme() const;

private:
    void createStyles(ryml::ConstNodeRef node, LoadingContext &context);
    Union::SelectorList createSelectors(ryml::ConstNodeRef node);
    Union::StyleRule::Ptr createStyle(const Union::SelectorList &selectors, ryml::ConstNodeRef node, LoadingContext &context);
    Union::Selector createSelector(ryml::ConstNodeRef node);

    std::optional<Union::Properties::LayoutProperty> createLayoutProperty(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::Properties::TextProperty> createTextProperty(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::Properties::IconProperty> createIconProperty(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::Properties::BackgroundProperty> createBackgroundProperty(ryml::ConstNodeRef node, LoadingContext &context);

    std::optional<Union::Properties::PaletteProperty> createPaletteProperty(LoadingContext &context);

    std::optional<Union::Properties::AlignmentProperty> createAlignmentProperty(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::Properties::SizeProperty> createSizeProperty(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::Properties::BorderProperty> createBorderProperty(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::Properties::CornersProperty> createCornersProperty(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::Properties::LineProperty> createLineProperty(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::Properties::CornerProperty> createCornerProperty(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::Properties::ImageProperty> createImageProperty(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::Properties::ShadowProperty> createShadowProperty(ryml::ConstNodeRef node, LoadingContext &context);

    Plasma::Theme m_plasmaTheme;
};
