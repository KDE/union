/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QObject>
#include <qqmlregistration.h>

#include "StyleElement.h"

namespace Union
{

class Element : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    Element(QObject *parent = nullptr);

    /**
     * TODO: Documentation
     */
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    QString name() const;
    void setName(const QString &newName);
    Q_SIGNAL void nameChanged();

    Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)
    bool hovered() const;
    void setHovered(bool newHovered);
    Q_SIGNAL void hoveredChanged();

    Q_PROPERTY(bool focus READ focus WRITE setFocus NOTIFY focusChanged)
    bool focus() const;
    void setFocus(bool newFocus);
    Q_SIGNAL void focusChanged();

    Q_PROPERTY(bool activeFocus READ activeFocus WRITE setActiveFocus NOTIFY activeFocusChanged)
    bool activeFocus() const;
    void setActiveFocus(bool newActiveFocus);
    Q_SIGNAL void activeFocusChanged();

    Q_PROPERTY(bool pressed READ pressed WRITE setPressed NOTIFY pressedChanged)
    bool pressed() const;
    void setPressed(bool newPressed);
    Q_SIGNAL void pressedChanged();

    // Q_PROPERTY(PaddingGroup * padding READ padding CONSTANT)
    // PaddingGroup * padding() const;
    //
    // Q_PROPERTY(MarginGroup * margin READ margin CONSTANT)
    // MarginGroup * margin() const;
    //
    // Q_PROPERTY(ShadowGroup * shadow READ shadow CONSTANT)
    // ShadowGroup * shadow() const;
    //
    // Q_PROPERTY(BorderGroup * border READ border CONSTANT)
    // BorderGroup * border() const;
    //
    // Q_PROPERTY(BackgroundGroup* background READ background CONSTANT)
    // BackgroundGroup* background() const;

    StyleElement::Ptr styleElement() const;
    Q_SIGNAL void stateChanged();

private:
    void updateElement();

    QString m_name;

    bool m_hovered = false;
    bool m_focus = false;
    bool m_activeFocus = false;
    bool m_pressed = false;

    StyleElement::Ptr m_normalElement;
    StyleElement::Ptr m_hoverElement;
    StyleElement::Ptr m_focusElement;
    StyleElement::Ptr m_activeFocusElement;
    StyleElement::Ptr m_pressElement;
};

}
