// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Martin Sh <hemisputnik@proton.me>

#include <QBoxLayout>
#include <QDateTimeEdit>
#include <QGridLayout>
#include <QSlider>
#include <QSpinBox>

#include "numberpage.h"

NumberPage::NumberPage(QWidget *parent)
    : WidgetPage(parent)
{
    auto *layout = new QVBoxLayout(this);
    auto *sliderLayout = new QGridLayout();

    auto *sliderH = new QSlider(Qt::Horizontal, this);
    sliderH->setMinimumWidth(200);
    sliderH->setMinimum(0);
    sliderH->setMaximum(5);

    auto *sliderV = new QSlider(Qt::Vertical, this);
    sliderV->setMinimumHeight(200);
    sliderV->setMinimum(0);
    sliderV->setMaximum(5);

    auto *sliderHTicks = new QSlider(Qt::Horizontal, this);
    sliderHTicks->setMinimumWidth(200);
    sliderHTicks->setMinimum(0);
    sliderHTicks->setMaximum(5);
    sliderHTicks->setTickInterval(1);
    sliderHTicks->setTickPosition(QSlider::TicksBelow);

    auto *sliderVTicks = new QSlider(Qt::Vertical, this);
    sliderVTicks->setMinimumHeight(200);
    sliderVTicks->setMinimum(0);
    sliderVTicks->setMaximum(5);
    sliderVTicks->setTickInterval(1);
    sliderVTicks->setTickPosition(QSlider::TicksRight);

    auto *sliderHTicks2 = new QSlider(Qt::Horizontal, this);
    sliderHTicks2->setMinimumWidth(200);
    sliderHTicks2->setMinimum(0);
    sliderHTicks2->setMaximum(5);
    sliderHTicks2->setTickInterval(1);
    sliderHTicks2->setTickPosition(QSlider::TicksAbove);

    auto *sliderVTicks2 = new QSlider(Qt::Vertical, this);
    sliderVTicks2->setMinimumHeight(200);
    sliderVTicks2->setMinimum(0);
    sliderVTicks2->setMaximum(5);
    sliderVTicks2->setTickInterval(1);
    sliderVTicks2->setTickPosition(QSlider::TicksLeft);

    sliderH->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    sliderV->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    sliderHTicks->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    sliderVTicks->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    sliderHTicks2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    sliderVTicks2->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    sliderLayout->addWidget(sliderH, 0, 0);
    sliderLayout->addWidget(sliderV, 1, 0);
    sliderLayout->addWidget(sliderHTicks, 0, 1);
    sliderLayout->addWidget(sliderVTicks, 1, 1);
    sliderLayout->addWidget(sliderHTicks2, 0, 2);
    sliderLayout->addWidget(sliderVTicks2, 1, 2);

    layout->addLayout(sliderLayout);

    auto *spinBox = new QSpinBox(this);
    auto *spinBoxText = new QSpinBox(this);
    spinBoxText->setPrefix(QStringLiteral("prefix "));
    spinBoxText->setSuffix(QStringLiteral(" suffix"));

    auto *timeEdit = new QTimeEdit(this);
    auto *dateEdit = new QDateEdit(this);
    auto *dateTimeEdit = new QDateTimeEdit(this);

    spinBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    spinBoxText->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    timeEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    dateEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    dateTimeEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    layout->addWidget(spinBox);
    layout->addWidget(spinBoxText);
    layout->addWidget(timeEdit);
    layout->addWidget(dateEdit);
    layout->addWidget(dateTimeEdit);

    layout->addStretch(1);
}

QString NumberPage::title()
{
    return QStringLiteral("Numeric Editors");
}
