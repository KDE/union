// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Martin Sh <hemisputnik@proton.me>

#pragma once

#include <QWidget>

class WidgetPage : public QWidget
{
    Q_OBJECT

public:
    WidgetPage(QWidget *parent = nullptr);

    virtual QString title() = 0;
};
