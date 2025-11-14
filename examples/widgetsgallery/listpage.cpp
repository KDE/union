// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Martin Sh <hemisputnik@proton.me>

#include <QBoxLayout>
#include <QFileSystemModel>
#include <QLabel>
#include <QListView>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeView>

#include "listpage.h"

ListPage::ListPage(QWidget *parent)
    : WidgetPage(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto *listModel = new QStandardItemModel(this);
    listModel->appendRow(new QStandardItem(QIcon::fromTheme(QStringLiteral("arrow-down")), QStringLiteral("Take the road to the end of the galaxy")));
    listModel->appendRow(new QStandardItem(QIcon::fromTheme(QStringLiteral("arrow-left")), QStringLiteral("Take care of your belongings and your fantasies")));
    listModel->appendRow(
        new QStandardItem(QIcon::fromTheme(QStringLiteral("arrow-up")), QStringLiteral("Hurry dear, get your engines running for your longest haul")));
    listModel->appendRow(new QStandardItem(QIcon::fromTheme(QStringLiteral("arrow-right")), QStringLiteral("No one said the stars would be leaving so soon")));
    auto *listItem1 = new QStandardItem(QStringLiteral("Nothing left, saving the words that were said:"));
    listItem1->setCheckable(true);
    auto *listItem2 = new QStandardItem(QStringLiteral("\"One chance to shed off your troubles, travel the rainbow road, with pride\""));
    listItem2->setCheckable(true);
    listItem2->setCheckState(Qt::Checked);
    auto *listItem3 = new QStandardItem(QStringLiteral("Nothing left, saving the words that were said:"));
    listItem3->setCheckable(true);
    listItem3->setUserTristate(true);
    listItem3->setCheckState(Qt::PartiallyChecked);
    auto *listItem4 = new QStandardItem(QStringLiteral("\"One chance to shed off your troubles, travel the rainbow road, with pride\""));
    listItem4->setCheckable(true);
    listItem4->setUserTristate(true);
    listModel->appendRow(listItem1);
    listModel->appendRow(listItem2);
    listModel->appendRow(listItem3);
    listModel->appendRow(listItem4);
    auto *listItem5 = new QStandardItem(QStringLiteral("Carry your head straight dear and never follow the lights"));
    listItem5->setEnabled(false);
    auto *listItem6 = new QStandardItem(QStringLiteral("The road may wind all around us, guiding the lost and forgotten"));
    listItem6->setEnabled(false);
    auto *listItem7 = new QStandardItem(QStringLiteral("Jettison all your excess payload, never follow the lights"));
    listItem7->setEnabled(false);
    auto *listItem8 = new QStandardItem(QStringLiteral("At rainbow's end I wait, for the end to choose our sky"));
    listItem8->setEnabled(false);
    listModel->appendRow(listItem5);
    listModel->appendRow(listItem6);
    listModel->appendRow(listItem7);
    listModel->appendRow(listItem8);

    auto *listView = new QListView(this);
    listView->setModel(listModel);

    auto *tableModel = new QStandardItemModel(6, 4, this);
    tableModel->setHeaderData(0, Qt::Horizontal, QStringLiteral("Type"));
    tableModel->setHeaderData(1, Qt::Horizontal, QStringLiteral("Wings"));
    tableModel->setHeaderData(2, Qt::Horizontal, QStringLiteral("Front Legs"));
    tableModel->setHeaderData(3, Qt::Horizontal, QStringLiteral("Hind Legs"));
    tableModel->setItem(0, 0, new QStandardItem(QStringLiteral("Western Dragon")));
    tableModel->setItem(0, 1, new QStandardItem(QStringLiteral("Yes")));
    tableModel->setItem(0, 2, new QStandardItem(QStringLiteral("Yes")));
    tableModel->setItem(0, 3, new QStandardItem(QStringLiteral("Yes")));
    tableModel->setItem(1, 0, new QStandardItem(QStringLiteral("Eastern Dragon")));
    tableModel->setItem(1, 1, new QStandardItem(QStringLiteral("No")));
    tableModel->setItem(1, 2, new QStandardItem(QStringLiteral("Yes")));
    tableModel->setItem(1, 3, new QStandardItem(QStringLiteral("Yes")));
    tableModel->setItem(2, 0, new QStandardItem(QStringLiteral("Wyvern")));
    tableModel->setItem(2, 1, new QStandardItem(QStringLiteral("Yes")));
    tableModel->setItem(2, 2, new QStandardItem(QStringLiteral("No")));
    tableModel->setItem(2, 3, new QStandardItem(QStringLiteral("Yes")));
    tableModel->setItem(3, 0, new QStandardItem(QStringLiteral("Wyrm")));
    tableModel->setItem(3, 1, new QStandardItem(QStringLiteral("No")));
    tableModel->setItem(3, 2, new QStandardItem(QStringLiteral("No")));
    tableModel->setItem(3, 3, new QStandardItem(QStringLiteral("No")));
    tableModel->setItem(4, 0, new QStandardItem(QStringLiteral("Lindworm")));
    tableModel->setItem(4, 1, new QStandardItem(QStringLiteral("No")));
    tableModel->setItem(4, 2, new QStandardItem(QStringLiteral("Yes")));
    tableModel->setItem(4, 3, new QStandardItem(QStringLiteral("No")));
    tableModel->setItem(5, 0, new QStandardItem(QStringLiteral("Amphiptere")));
    tableModel->setItem(5, 1, new QStandardItem(QStringLiteral("Yes")));
    tableModel->setItem(5, 2, new QStandardItem(QStringLiteral("No")));
    tableModel->setItem(5, 3, new QStandardItem(QStringLiteral("No")));

    auto *tablesLayout = new QHBoxLayout();

    auto *tableView = new QTableView(this);
    tableView->setModel(tableModel);
    tableView->setColumnWidth(0, 200);

    auto *tableView2 = new QTableView(this);
    tableView2->setModel(tableModel);
    tableView2->setColumnWidth(0, 200);
    tableView2->setShowGrid(false);

    tablesLayout->addWidget(tableView);
    tablesLayout->addWidget(tableView2);

    auto *fsModel = new QFileSystemModel(this);
    fsModel->setRootPath(QDir::currentPath());
    auto *treeView = new QTreeView(this);
    treeView->setModel(fsModel);

    auto *tabWidget = new QTabWidget(this);
    tabWidget->addTab(new QLabel(QStringLiteral("One day")), QIcon::fromTheme(QStringLiteral("document-save")), QStringLiteral("Tab 1"));
    tabWidget->addTab(new QLabel(QStringLiteral("I went to the park")), QIcon::fromTheme(QStringLiteral("document-save")), QStringLiteral("Tab 2"));
    tabWidget->addTab(new QLabel(QStringLiteral("The park had")), QStringLiteral("Tab 3"));
    tabWidget->addTab(new QLabel(QStringLiteral("Many pine trees")), QStringLiteral("Tab 4"));
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);

    layout->addWidget(listView);
    layout->addLayout(tablesLayout);
    layout->addWidget(treeView);
    layout->addWidget(tabWidget);

    layout->addStretch(1);
}

QString ListPage::title()
{
    return QStringLiteral("Lists");
}
