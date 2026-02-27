// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#pragma once

#include "Color.h"

#include "unionplatform_export.h"

class UNIONPLATFORM_EXPORT KColorSchemeProvider : public Union::ColorProvider
{
    Q_OBJECT

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
