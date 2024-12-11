// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QMainWindow mainWindow;
    mainWindow.setWindowTitle(QStringLiteral("Union Widgets"));

    QWidget widget;
    mainWindow.setCentralWidget(&widget);

    QVBoxLayout layout;
    widget.setLayout(&layout);

    QPushButton button(QStringLiteral("Foo"));
    layout.addWidget(&button);

    QPushButton button2(QStringLiteral("Bar"));
    layout.addWidget(&button2);

    QPushButton button3(QStringLiteral("Disabled"));
    button3.setEnabled(false);
    layout.addWidget(&button3);

    mainWindow.show();

    return a.exec();
}
