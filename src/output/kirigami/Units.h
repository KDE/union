// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <Kirigami/Platform/Units>

class Units : public Kirigami::Platform::Units
{
    Q_OBJECT
public:
    explicit Units(QObject *parent = nullptr);
};
