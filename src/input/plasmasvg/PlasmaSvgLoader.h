/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <Plasma/Theme>

#include <yaml-cpp/yaml.h>

#include <StyleElement.h>

#include "StyleLoader.h"

class QSvgRenderer;

namespace Union
{
struct CornerDefinition;
struct LineDefinition;
struct ImageDefinition;
}

struct LoaderContext;

class PlasmaSvgLoader : public Union::StyleLoader
{
    Q_OBJECT

public:
    PlasmaSvgLoader(std::shared_ptr<Union::Style> style, QObject *parent = nullptr);

    bool load() override;

private:
    std::shared_ptr<Union::StyleElement> createElement(const YAML::Node &node, QStack<LoaderContext> &context);
    std::optional<Union::LineDefinition> createLineDefinition(std::shared_ptr<QSvgRenderer> renderer,
                                                              const QString &elementName,
                                                              Qt::Orientation orientation,
                                                              Union::LineDefinition parentLine);
    std::optional<Union::CornerDefinition> createCornerDefinition(std::shared_ptr<QSvgRenderer> renderer,
                                                                  const QString &elementName,
                                                                  Union::CornerDefinition parentCorner);
    std::optional<Union::ImageDefinition> renderElement(std::shared_ptr<QSvgRenderer> renderer,
                                                        const QString &elementName,
                                                        const Union::ImageDefinition &parentImage);
    QSizeF elementSize(std::shared_ptr<QSvgRenderer> renderer, const QString &elementName);

    Plasma::Theme m_theme;
};
