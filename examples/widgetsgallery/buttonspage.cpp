// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Martin Sh <hemisputnik@proton.me>

#include <QGridLayout>
#include <QPushButton>
#include <QStringLiteral>
#include <QToolButton>

#include "buttonspage.h"

ButtonsPage::ButtonsPage(QWidget *parent)
    : WidgetPage(parent)
{
    auto *layout = new QGridLayout(this);

    const auto icon = QIcon::fromTheme(QStringLiteral("document-save"));

    auto *buttonText = new QPushButton(QStringLiteral("Button with Text (with tooltip)"), this);
    buttonText->setToolTip(QStringLiteral("A tooltip"));
    auto *buttonIcon = new QPushButton(icon, QStringLiteral("Button with Text and Icon"), this);
    auto *buttonIconOnly = new QPushButton(this);
    buttonIconOnly->setIcon(icon);
    auto *buttonDisabled = new QPushButton(icon, QStringLiteral("Disabled Button"), this);
    buttonDisabled->setEnabled(false);
    auto *buttonFlat = new QPushButton(icon, QStringLiteral("Button Flat"), this);
    buttonFlat->setFlat(true);
    auto *buttonCheckable = new QPushButton(icon, QStringLiteral("Button Checkable"), this);
    buttonCheckable->setCheckable(true);

    auto *toolButtonText = new QToolButton(this);
    toolButtonText->setText(QStringLiteral("ToolButton with Text (with tooltip)"));
    toolButtonText->setToolTip(QStringLiteral("A tooltip"));
    auto *toolButtonIcon = new QToolButton(this);
    toolButtonIcon->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolButtonIcon->setIcon(icon);
    toolButtonIcon->setText(QStringLiteral("ToolButton with Text and Icon"));
    auto *toolButtonIconOnly = new QToolButton(this);
    toolButtonIconOnly->setIcon(icon);
    auto *toolButtonDisabled = new QToolButton(this);
    toolButtonDisabled->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolButtonDisabled->setIcon(icon);
    toolButtonDisabled->setText(QStringLiteral("Disabled ToolButton"));
    toolButtonDisabled->setEnabled(false);
    auto *toolButtonCheckable = new QToolButton(this);
    toolButtonCheckable->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolButtonCheckable->setIcon(icon);
    toolButtonCheckable->setText(QStringLiteral("ToolButton Checkable"));
    toolButtonCheckable->setCheckable(true);
    auto *toolButtonTextBelow = new QToolButton(this);
    toolButtonTextBelow->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolButtonTextBelow->setIcon(icon);
    toolButtonTextBelow->setText(QStringLiteral("ToolButton Text below Icon"));

    buttonText->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    buttonIcon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    buttonIconOnly->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    buttonDisabled->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    buttonFlat->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    buttonCheckable->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    toolButtonText->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    toolButtonIcon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    toolButtonIconOnly->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    toolButtonDisabled->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    toolButtonCheckable->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    toolButtonTextBelow->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    layout->addWidget(buttonText, 0, 0);
    layout->addWidget(buttonIcon, 1, 0);
    layout->addWidget(buttonIconOnly, 2, 0);
    layout->addWidget(buttonDisabled, 3, 0);
    layout->addWidget(buttonFlat, 4, 0);
    layout->addWidget(buttonCheckable, 5, 0);
    layout->addWidget(toolButtonText, 0, 1);
    layout->addWidget(toolButtonIcon, 1, 1);
    layout->addWidget(toolButtonIconOnly, 2, 1);
    layout->addWidget(toolButtonDisabled, 3, 1);
    layout->addWidget(toolButtonCheckable, 5, 1);
    layout->addWidget(toolButtonTextBelow, 6, 1);
}

QString ButtonsPage::title()
{
    return QStringLiteral("Buttons");
}
