/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QQuickItem>
#include <qqmlregistration.h>

#include "QuickElement.h"

namespace Union
{

class Background : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    Background(QQuickItem *parent = nullptr);

    Q_PROPERTY(QuickElement *element READ element WRITE setElement NOTIFY elementChanged)
    QuickElement *element() const;
    void setElement(QuickElement *element);
    Q_SIGNAL void elementChanged();

    void componentComplete() override;

protected:
    QSGNode *updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *data) override;

private:
    QuickElement *m_element = nullptr;
};

}
