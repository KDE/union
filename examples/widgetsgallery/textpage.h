// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Martin Sh <hemisputnik@proton.me>

#pragma once

#include "widgetpage.h"

class TextPage : public WidgetPage
{
    Q_OBJECT

public:
    TextPage(QWidget *parent = nullptr);

    QString title() override;
};
