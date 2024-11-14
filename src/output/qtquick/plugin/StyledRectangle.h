/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QQuickItem>
#include <qqmlregistration.h>

#include "QuickStyle.h"

/**
 * An item used to render a rectangle based on style properties.
 *
 * This will render a rectangle for an element based on the properties exposed
 * to QuickStyle.
 */
class StyledRectangle : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    StyledRectangle(QQuickItem *parent = nullptr);

    void componentComplete() override;

protected:
    bool event(QEvent *event) override;
    QSGNode *updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *data) override;

private:
    void updateImplicitSize();

    QuickStyle *m_style = nullptr;
};
