// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Martin Sh <hemisputnik@proton.me>

#include <QBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QStringLiteral>
#include <QTextEdit>

#include "textpage.h"

TextPage::TextPage(QWidget *parent)
    : WidgetPage(parent)
{
    auto *layout = new QVBoxLayout(this);
    setLayout(layout);

    auto *label = new QLabel(this);
    label->setText(QStringLiteral("Sphinx of black quartz, judge my vow."));

    auto *lineEdit = new QLineEdit(this);
    lineEdit->setPlaceholderText(QStringLiteral("QLineEdit"));

    auto *textEdit = new QTextEdit(this);
    textEdit->setPlaceholderText(QStringLiteral("QTextEdit"));
    textEdit->setLineWrapMode(QTextEdit::NoWrap);
    textEdit->setText(
        QStringLiteral("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
                       "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"
                       "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n"
                       "Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.\n"
                       "\n\n\n\n\n\n\n\n\n\n\n\n"
                       "Hello"));

    auto *model = new QStandardItemModel(this);
    model->appendRow(new QStandardItem(QStringLiteral("Red")));
    model->appendRow(new QStandardItem(QStringLiteral("Yellow")));
    model->appendRow(new QStandardItem(QStringLiteral("Kitchen utensil")));
    model->appendRow(
        new QStandardItem(QIcon::fromTheme(QStringLiteral("document-save")), QStringLiteral("One day I went to the park. The park had many pine trees.")));

    auto *comboBox = new QComboBox(this);
    comboBox->setModel(model);

    auto *comboBoxEditable = new QComboBox(this);
    comboBoxEditable->setEditable(true);
    comboBoxEditable->setModel(model);

    auto *comboBoxNoFrame = new QComboBox(this);
    comboBoxNoFrame->setFrame(false);
    comboBoxNoFrame->setModel(model);

    auto *comboBoxNoFrameEditable = new QComboBox(this);
    comboBoxNoFrameEditable->setEditable(true);
    comboBoxNoFrameEditable->setFrame(false);
    comboBoxNoFrameEditable->setModel(model);

    label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    lineEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    textEdit->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    comboBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    comboBoxEditable->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    comboBoxNoFrame->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    comboBoxNoFrameEditable->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(textEdit);
    layout->addWidget(comboBox);
    layout->addWidget(comboBoxEditable);
    layout->addWidget(comboBoxNoFrame);
    layout->addWidget(comboBoxNoFrameEditable);
    layout->addStretch(1);
}

QString TextPage::title()
{
    return QStringLiteral("Text");
}
