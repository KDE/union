/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QMetaEnum>

#include <Plasma/Theme>

#define RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS
#include <ryml.hpp>
#include <ryml_std.hpp>

#include <Element.h>

#include "StyleLoader.h"

class QSvgRenderer;

namespace Union
{
struct CornerDefinition;
struct LineDefinition;
struct ImageDefinition;
}

struct LoadingContext;

class PlasmaSvgLoader : public Union::StyleLoader
{
    Q_OBJECT

public:
    PlasmaSvgLoader(std::shared_ptr<Union::Theme> theme, QObject *parent = nullptr);

    bool load() override;

private:
    void createStyles(ryml::ConstNodeRef node, LoadingContext &context);
    Union::Style::Ptr createStyle(ryml::ConstNodeRef node, LoadingContext &context);

    std::optional<Union::SizeDefinition> createSizeDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::BorderDefinition> createBorderDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::CornersDefinition> createCornersDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::AreaDefinition> createAreaDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::LineDefinition> createLineDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::CornerDefinition> createCornerDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::ImageDefinition> createImageDefinition(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<Union::ShadowDefinition> createShadowDefinition(ryml::ConstNodeRef node, LoadingContext &context);

    QVariant elementProperty(ryml::ConstNodeRef node, LoadingContext &context);

    QVariant constantValue(ryml::ConstNodeRef node, LoadingContext &context);
    std::optional<QSizeF> elementSize(ryml::ConstNodeRef node, LoadingContext &context);
    QImage elementImage(ryml::ConstNodeRef node, LoadingContext &context);
    QImage elementImageBlend(ryml::ConstNodeRef node, LoadingContext &context);

    std::shared_ptr<QSvgRenderer> rendererForPath(QAnyStringView path);

    Plasma::Theme m_theme;
    QHash<QString, std::shared_ptr<QSvgRenderer>> m_renderers;
};
