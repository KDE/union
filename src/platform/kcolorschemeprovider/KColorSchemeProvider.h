// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#pragma once

#include "Color.h"

class KColorSchemeProvider : public Union::ColorProvider
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.union.ColorProvider" FILE "kcolorschemeprovider.json")

public:
    KColorSchemeProvider(QObject *parent = nullptr);
    ~KColorSchemeProvider() override;

    std::optional<Union::ColorProvider::Rgba> color(const QStringList &arguments) const override;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    class Private;
    const std::unique_ptr<Private> d;
};
