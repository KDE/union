/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <optional>

#include <QHash>

#include <Plasma/Theme>

#include "StyleLoader.h"

class QSvgRenderer;

namespace Union
{
struct CornerDefinition;
struct LineDefinition;
struct ImageDefinition;
}

class PlasmaSvgLoader : public Union::StyleLoader
{
    Q_OBJECT

public:
    PlasmaSvgLoader(std::shared_ptr<Union::Style> style, QObject *parent = nullptr);

    bool load() override;

private:
    std::shared_ptr<Union::StyleElement> createElement(const QString &elementName, const QString &prefix);
    std::optional<Union::LineDefinition> createLineDefinition(QSvgRenderer &renderer, const QString &elementName);
    std::optional<Union::CornerDefinition> createCornerDefinition(QSvgRenderer &renderer, const QString &elementName);
    std::optional<Union::ImageDefinition> renderElement(QSvgRenderer &renderer, const QString &elementName);

    Plasma::Theme m_theme;
};
