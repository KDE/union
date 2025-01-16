// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <InputPlugin.h>

namespace Union
{
class Theme;
}

class PlasmaSvgPlugin : public Union::InputPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.union.InputPlugin" FILE "metadata.json")

public:
    PlasmaSvgPlugin(QObject *parent = nullptr);

    std::shared_ptr<Union::Theme> createTheme(const QString &themeName) const override;
};
