/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <InputPlugin.h>

class PlasmaSvgPlugin : public Union::InputPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.union.InputPlugin" FILE "metadata.json")

public:
    PlasmaSvgPlugin(QObject *parent = nullptr);

    std::unique_ptr<Union::StyleLoader> createStyleLoader(std::shared_ptr<Union::Style> style) const override;
};
