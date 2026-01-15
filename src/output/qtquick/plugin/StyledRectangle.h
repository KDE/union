// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QQuickItem>
#include <qqmlregistration.h>

#include "QuickStyle.h"

/*!
 * \qmltype StyledRectangle
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-core
 *
 * \brief An item used to render a rectangle based on style properties.
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
    QSGNode *updateRectangleNode(QSGNode *node, const Union::Properties::StyleProperty *style);
    QSGNode *updateShaderNode(QSGNode *node, const Union::Properties::StyleProperty *style);
    QSizeF minimumSize(const Union::Properties::StyleProperty *background);

    QuickStyle *m_style = nullptr;
};
