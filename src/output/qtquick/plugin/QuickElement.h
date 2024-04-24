/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QMarginsF>
#include <QObject>
#include <QProperty>
#include <QQuickAttachedPropertyPropagator>
#include <qqmlregistration.h>

#include <Element.h>
#include <ElementQuery.h>
#include <Selector.h>

class QuickElement;

class Sizes
{
    Q_GADGET
    QML_VALUE_TYPE(sizes)

public:
    Sizes();
    Sizes(qreal left, qreal right, qreal top, qreal bottom);
    Sizes(const QMarginsF &margins);

    bool operator==(const Sizes &other);

    Q_PROPERTY(qreal left READ left WRITE setLeft)
    qreal left() const;
    void setLeft(qreal newLeft);

    Q_PROPERTY(qreal right READ right WRITE setRight)
    qreal right() const;
    void setRight(qreal newRight);

    Q_PROPERTY(qreal top READ top WRITE setTop)
    qreal top() const;
    void setTop(qreal newTop);

    Q_PROPERTY(qreal bottom READ bottom WRITE setBottom)
    qreal bottom() const;
    void setBottom(qreal newBottom);

    Q_PROPERTY(bool valid READ isValid)
    bool isValid() const;

private:
    QMarginsF m_margins;
};

class BordersGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    BordersGroup();

    Q_PROPERTY(Sizes sizes READ sizes NOTIFY sizesChanged BINDABLE bindableSizes)
    Sizes sizes() const;
    QBindable<Sizes> bindableSizes();
    Q_SIGNAL void sizesChanged();

private:
    void onUpdated();

    Q_OBJECT_BINDABLE_PROPERTY(BordersGroup, Sizes, m_sizes, &BordersGroup::sizesChanged)
};

class StatesGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    StatesGroup();

    Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged BINDABLE bindableHovered)
    bool hovered() const;
    void setHovered(bool newHovered);
    QBindable<bool> bindableHovered();
    Q_SIGNAL void hoveredChanged();

    Q_PROPERTY(bool focus READ focus WRITE setFocus NOTIFY focusChanged BINDABLE bindableFocus)
    bool focus() const;
    void setFocus(bool newFocus);
    QBindable<bool> bindableFocus();
    Q_SIGNAL void focusChanged();

    Q_PROPERTY(bool activeFocus READ activeFocus WRITE setActiveFocus NOTIFY activeFocusChanged BINDABLE bindableActiveFocus)
    bool activeFocus() const;
    void setActiveFocus(bool newActiveFocus);
    QBindable<bool> bindableActiveFocus();
    Q_SIGNAL void activeFocusChanged();

    Q_PROPERTY(bool pressed READ pressed WRITE setPressed NOTIFY pressedChanged BINDABLE bindablePressed)
    bool pressed() const;
    void setPressed(bool newPressed);
    QBindable<bool> bindablePressed();
    Q_SIGNAL void pressedChanged();

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged BINDABLE bindableEnabled)
    bool enabled() const;
    void setEnabled(bool newEnabled);
    QBindable<bool> bindableEnabled();
    Q_SIGNAL void enabledChanged();

    Q_PROPERTY(QStringList activeStates READ activeStates STORED false NOTIFY activeStatesChanged BINDABLE bindableActiveStates)
    QStringList activeStates() const;
    QBindable<QStringList> bindableActiveStates();
    Q_SIGNAL void activeStatesChanged();

private:
    Q_OBJECT_BINDABLE_PROPERTY(StatesGroup, bool, m_hovered, &StatesGroup::hoveredChanged)
    Q_OBJECT_BINDABLE_PROPERTY(StatesGroup, bool, m_focus, &StatesGroup::focusChanged)
    Q_OBJECT_BINDABLE_PROPERTY(StatesGroup, bool, m_activeFocus, &StatesGroup::activeFocusChanged)
    Q_OBJECT_BINDABLE_PROPERTY(StatesGroup, bool, m_pressed, &StatesGroup::pressedChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(StatesGroup, bool, m_enabled, true, &StatesGroup::enabledChanged)
    Q_OBJECT_BINDABLE_PROPERTY(StatesGroup, QStringList, m_activeStates, &StatesGroup::activeStatesChanged)
};

class QuickElement : public QQuickAttachedPropertyPropagator
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Element)
    QML_ATTACHED(QuickElement)

public:
    QuickElement(QObject *parent = nullptr);

    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged BINDABLE bindableType)
    QString type() const;
    void setType(const QString &newType);
    QBindable<QString> bindableType();
    Q_SIGNAL void typeChanged();

    Q_PROPERTY(QString elementId READ elementId WRITE setElementId NOTIFY elementIdChanged BINDABLE bindableElementId)
    QString elementId() const;
    void setElementId(const QString &newId);
    QBindable<QString> bindableElementId();
    Q_SIGNAL void elementIdChanged();

    Q_PROPERTY(StatesGroup *states READ states CONSTANT)
    StatesGroup *states() const;

    Q_PROPERTY(QStringList hints READ hints WRITE setHints NOTIFY hintsChanged BINDABLE bindableHints)
    QStringList hints() const;
    void setHints(const QStringList &newHints);
    QBindable<QStringList> bindableHints();
    Q_SIGNAL void hintsChanged();

    Q_PROPERTY(QVariantMap attributes READ attributes WRITE setAttributes NOTIFY attributesChanged BINDABLE bindableAttributes)
    QVariantMap attributes() const;
    void setAttributes(const QVariantMap &newAttributes);
    QBindable<QVariantMap> bindableAttributes();
    Q_SIGNAL void attributesChanged();

    Q_PROPERTY(qreal implicitWidth READ implicitWidth NOTIFY implicitWidthChanged BINDABLE bindableImplicitWidth)
    qreal implicitWidth() const;
    QBindable<qreal> bindableImplicitWidth();
    Q_SIGNAL void implicitWidthChanged();

    Q_PROPERTY(qreal implicitHeight READ implicitHeight NOTIFY implicitHeightChanged BINDABLE bindableImplicitHeight)
    qreal implicitHeight() const;
    QBindable<qreal> bindableImplicitHeight();
    Q_SIGNAL void implicitHeightChanged();

    Q_PROPERTY(Sizes padding READ padding NOTIFY paddingChanged BINDABLE bindablePadding)
    Sizes padding() const;
    QBindable<Sizes> bindablePadding();
    Q_SIGNAL void paddingChanged();

    Q_PROPERTY(Sizes margins READ margins NOTIFY marginsChanged BINDABLE bindableMargins)
    Sizes margins() const;
    QBindable<Sizes> bindableMargins();
    Q_SIGNAL void marginsChanged();

    Q_PROPERTY(BordersGroup *borders READ borders CONSTANT)
    BordersGroup *borders() const;
    //
    // Q_PROPERTY(ShadowGroup * shadow READ shadow CONSTANT)
    // ShadowGroup * shadow() const;
    //
    //
    // Q_PROPERTY(BackgroundGroup* background READ background CONSTANT)
    // BackgroundGroup* background() const;

    // Union::Element::Ptr styleElement() const;
    Union::ElementQuery buildQuery() const;

    Q_SIGNAL void updated();

    static QuickElement *qmlAttachedProperties(QObject *parent);

private:
    friend class StatesGroup;

    void update();

    Q_OBJECT_BINDABLE_PROPERTY(QuickElement, qreal, m_implicitWidth, &QuickElement::implicitWidthChanged)
    Q_OBJECT_BINDABLE_PROPERTY(QuickElement, qreal, m_implicitHeight, &QuickElement::implicitHeightChanged)
    Q_OBJECT_BINDABLE_PROPERTY(QuickElement, Sizes, m_padding, &QuickElement::paddingChanged)
    Q_OBJECT_BINDABLE_PROPERTY(QuickElement, Sizes, m_margins, &QuickElement::marginsChanged)

    std::shared_ptr<Union::Element> m_element;
    std::unique_ptr<BordersGroup> m_bordersGroup;
    std::unique_ptr<StatesGroup> m_statesGroup;

    QPropertyNotifier m_activeStatesNotifier;
};
