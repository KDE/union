/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include "StyleLoader.h"

class PlasmaSvgLoader : public Union::StyleLoader
{
    Q_OBJECT

public:
    PlasmaSvgLoader(QObject* parent = nullptr);

    bool load(const QUrl &url) override;

    std::shared_ptr<Union::StyleElement> get(const Union::ElementSelector &selector) override;
};
