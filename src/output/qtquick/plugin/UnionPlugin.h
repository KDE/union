// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QQmlEngineExtensionPlugin>

class UnionPlugin : public QQmlEngineExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)

public:
    UnionPlugin(QObject *parent = nullptr);

    void initializeEngine(QQmlEngine *, const char *) override;
};

void qml_register_types_org_kde_union_impl();
