/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QQuickItem>
#include <qqmlregistration.h>

#include "Element.h"

namespace Union
{

class Background : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    Background(QQuickItem *parent = nullptr);

    /**
     * TODO: Documentation
     */
    Q_PROPERTY(Element *element READ element WRITE setElement NOTIFY elementChanged)
    Element *element() const;
    void setElement(Element *newElement);
    Q_SIGNAL void elementChanged();

protected:
    // void stateChanged(ElementState::StateChange change) override;
    QSGNode *updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *data) override;

private:
    std::unique_ptr<Element> m_internalElement;
    Element *m_elementOverride = nullptr;
};

}
