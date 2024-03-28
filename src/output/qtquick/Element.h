/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QObject>
#include <qqmlregistration.h>

#include "StyleElement.h"

class Element;

class BordersGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    BordersGroup(Element *parent);

    Q_PROPERTY(qreal leftSize READ leftSize NOTIFY changed)
    qreal leftSize() const;

    Q_PROPERTY(qreal rightSize READ rightSize NOTIFY changed)
    qreal rightSize() const;

    Q_PROPERTY(qreal topSize READ topSize NOTIFY changed)
    qreal topSize() const;

    Q_PROPERTY(qreal bottomSize READ bottomSize NOTIFY changed)
    qreal bottomSize() const;

    Q_SIGNAL void changed();

private:
    Element *m_parent;
};

class PaddingGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    PaddingGroup(Element *parent);

    Q_PROPERTY(qreal left READ left NOTIFY changed)
    qreal left() const;

    Q_PROPERTY(qreal right READ right NOTIFY changed)
    qreal right() const;

    Q_PROPERTY(qreal top READ top NOTIFY changed)
    qreal top() const;

    Q_PROPERTY(qreal bottom READ bottom NOTIFY changed)
    qreal bottom() const;

    Q_SIGNAL void changed();

private:
    Element *m_parent;
};

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

    //
    // Q_PROPERTY(MarginGroup * margin READ margin CONSTANT)

    Q_PROPERTY(PaddingGroup *padding READ padding CONSTANT)
    PaddingGroup *padding() const;

    // MarginGroup * margin() const;

    Q_PROPERTY(BordersGroup *borders READ borders CONSTANT)
    BordersGroup *borders() const;
    //
    // Q_PROPERTY(ShadowGroup * shadow READ shadow CONSTANT)
    // ShadowGroup * shadow() const;
    //
    //
    // Q_PROPERTY(BackgroundGroup* background READ background CONSTANT)
    // BackgroundGroup* background() const;

    Q_PROPERTY(qreal implicitWidth READ implicitWidth CONSTANT)
    qreal implicitWidth() const;

    Q_PROPERTY(qreal implicitHeight READ implicitHeight CONSTANT)
    qreal implicitHeight() const;

    Union::StyleElement::Ptr styleElement() const;
    Q_SIGNAL void stateChanged();

private:
    void updateElement();

    QString m_name;

    bool m_hovered = false;
    bool m_focus = false;
    bool m_activeFocus = false;
    bool m_pressed = false;

    Union::StyleElement::Ptr m_normalElement;
    Union::StyleElement::Ptr m_hoverElement;
    Union::StyleElement::Ptr m_focusElement;
    Union::StyleElement::Ptr m_activeFocusElement;
    Union::StyleElement::Ptr m_pressElement;

    std::unique_ptr<PaddingGroup> m_paddingGroup;
    std::unique_ptr<BordersGroup> m_bordersGroup;
};
