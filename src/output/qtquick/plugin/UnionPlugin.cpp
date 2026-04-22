// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "UnionPlugin.h"
#include <QGuiApplication>
#include <QQuickWindow>
#include <StyleRegistry.h>

UnionPlugin::UnionPlugin(QObject *parent)
    : QQmlEngineExtensionPlugin(parent)
{
    // NativeTextRendering is still distorted sometimes with fractional scale factors
    // Given Qt disables all hinting with native rendering when any scaling is used anyway
    // we can use Qt's rendering throughout
    // QTBUG-126577
    if (qApp->devicePixelRatio() == 1.0) {
        QQuickWindow::setTextRenderType(QQuickWindow::NativeTextRendering);
    } else {
        QQuickWindow::setTextRenderType(QQuickWindow::QtTextRendering);
    }
}

void UnionPlugin::initializeEngine(QQmlEngine *, const char *)
{
    volatile auto registration = &qml_register_types_org_kde_union_impl;
    Q_UNUSED(registration);

    Union::StyleRegistry::instance()->load();
}

#include "moc_UnionPlugin.cpp"
