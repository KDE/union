// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Martin Sh <hemisputnik@proton.me>

#include <QBoxLayout>
#include <QListView>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QtLogging>
#include <qloggingcategory.h>
#include <qstringliteral.h>

#include "buttonspage.h"
#include "checkablepage.h"
#include "listpage.h"
#include "mainwindow.h"
#include "numberpage.h"
#include "textpage.h"
#include "widgetpage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1280, 720);
    setWindowTitle(QStringLiteral("Union QtWidgets Gallery"));

    auto *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto *mainLayout = new QHBoxLayout();
    centralWidget->setLayout(mainLayout);

    QList<WidgetPage *> pages;
    pages.append(new ButtonsPage(this));
    pages.append(new TextPage(this));
    pages.append(new NumberPage(this));
    pages.append(new CheckablePage(this));
    pages.append(new ListPage(this));

    auto *tabModel = new QStandardItemModel(this);
    auto *contents = new QStackedWidget(this);

    for (auto *page : pages) {
        auto *item = new QStandardItem(page->title());
        item->setEditable(false);
        tabModel->appendRow(item);
        contents->addWidget(page);
    }

    auto *tabList = new QListView(this);
    tabList->setModel(tabModel);
    connect(tabList, &QListView::clicked, this, [contents](QModelIndex index) {
        contents->setCurrentIndex(index.row());
    });

    mainLayout->addWidget(tabList);
    mainLayout->addWidget(contents, 1);
}
