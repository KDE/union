// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Martin Sh <hemisputnik@proton.me>

#include <QBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QRadioButton>

#include "checkablepage.h"

CheckablePage::CheckablePage(QWidget *parent)
    : WidgetPage(parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *radioGroup = new QGroupBox(QStringLiteral("Radio Buttons"), this);
    auto *radioLayout = new QVBoxLayout();

    auto *radio1 = new QRadioButton(QStringLiteral("Tomato Soup"), this);
    auto *radio2 = new QRadioButton(QStringLiteral("Split Pea Soup"), this);
    auto *radio3 = new QRadioButton(QStringLiteral("Chicken Soup"), this);
    auto *radio4 = new QRadioButton(QStringLiteral("Soup of the Day"), this);

    radio1->setChecked(true);

    radioLayout->addWidget(radio1);
    radioLayout->addWidget(radio2);
    radioLayout->addWidget(radio3);
    radioLayout->addWidget(radio4);

    radioGroup->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    radioGroup->setLayout(radioLayout);
    layout->addWidget(radioGroup);

    auto *checkboxGroup = new QGroupBox(QStringLiteral("Checkboxes"), this);
    auto *checkboxLayout = new QVBoxLayout();

    auto *checkbox1 = new QCheckBox(QStringLiteral("Tomatoes"), this);
    auto *checkbox2 = new QCheckBox(QStringLiteral("Olives"), this);
    checkbox2->setChecked(true);
    auto *checkbox3 = new QCheckBox(QStringLiteral("Mushrooms"), this);
    checkbox3->setCheckState(Qt::PartiallyChecked);
    checkbox3->setTristate(true);
    auto *checkbox4 = new QCheckBox(QStringLiteral("Extra Cheese"), this);
    checkbox4->setTristate(true);

    checkboxLayout->addWidget(checkbox1);
    checkboxLayout->addWidget(checkbox2);
    checkboxLayout->addWidget(checkbox3);
    checkboxLayout->addWidget(checkbox4);

    checkboxGroup->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    checkboxGroup->setLayout(checkboxLayout);
    layout->addWidget(checkboxGroup);

    layout->addStretch(1);
}

QString CheckablePage::title()
{
    return QStringLiteral("Checkable");
}
