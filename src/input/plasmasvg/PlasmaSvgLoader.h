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

struct RendererId {
    QString path;
    QPalette::ColorGroup colorGroup;
    Union::Element::ColorSet colorSet;

    inline bool operator==(const RendererId &other) const
    {
        return path == other.path && colorGroup == other.colorGroup && colorSet == other.colorSet;
    }
};

class PlasmaSvgLoader : public Union::ThemeLoader
{
public:
    bool load(std::shared_ptr<Union::Theme> theme) override;

private:
    void createStyles(ryml::ConstNodeRef node, LoadingContext &context);
    Union::StyleRule::Ptr createStyle(ryml::ConstNodeRef node, LoadingContext &context);
    Union::Selector createSelector(ryml::ConstNodeRef node);

    std::optional<Union::SizeDefinition> createSizeDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::BorderDefinition> createBorderDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::CornersDefinition> createCornersDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::AreaDefinition> createAreaDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::LineDefinition> createLineDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::CornerDefinition> createCornerDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::ImageDefinition> createImageDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::ShadowDefinition> createShadowDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::TextDefinition> createTextDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::IconDefinition> createIconDefinition(ryml::ConstNodeRef node, LoadingContext &context);

    QVariant elementProperty(ryml::ConstNodeRef node, LoadingContext &context);

    std::optional<QSizeF> elementSize(ryml::ConstNodeRef node, LoadingContext &context);
    QImage elementImage(ryml::ConstNodeRef node, LoadingContext &context);
    QImage elementImageBlend(ryml::ConstNodeRef node, LoadingContext &context);

    std::shared_ptr<PlasmaSvgRenderer> rendererForPath(QAnyStringView path, QPalette::ColorGroup colorGroup, Union::Element::ColorSet colorSet);

    Plasma::Theme m_theme;
    QHash<RendererId, std::shared_ptr<PlasmaSvgRenderer>> m_renderers;
};

std::size_t qHash(const RendererId &renderer, std::size_t seed = 0);
