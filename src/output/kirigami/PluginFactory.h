// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <Kirigami/Platform/PlatformPluginFactory>

#include <QObject>

class PluginFactory : public Kirigami::Platform::PlatformPluginFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PlatformPluginFactory_iid)
    Q_INTERFACES(Kirigami::Platform::PlatformPluginFactory)

public:
    explicit PluginFactory(QObject *parent = nullptr);

    Kirigami::Platform::Units *createUnits(QObject *parent) override;
};
