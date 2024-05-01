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

#include "Types.h"

class QuickElement;

class BordersGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    BordersGroup();

    Q_PROPERTY(Sizes sizes READ sizes BINDABLE bindableSizes NOTIFY sizesChanged)
    Sizes sizes() const;
    QBindable<Sizes> bindableSizes();
    Q_SIGNAL void sizesChanged();

    void update(const std::optional<Union::BorderDefinition> &borders);

private:
    Q_OBJECT_BINDABLE_PROPERTY(BordersGroup, Sizes, m_sizes, &BordersGroup::sizesChanged)
};

class TextGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    TextGroup();

    Q_PROPERTY(Qt::Alignment horizontalAlignment READ horizontalAlignment BINDABLE bindableHorizontalAlignment NOTIFY horizontalAlignmentChanged)
    Qt::Alignment horizontalAlignment() const;
    QBindable<Qt::Alignment> bindableHorizontalAlignment();
    Q_SIGNAL void horizontalAlignmentChanged();

    Q_PROPERTY(Qt::Alignment verticalAlignment READ verticalAlignment BINDABLE bindableVerticalAlignment NOTIFY verticalAlignmentChanged)
    Qt::Alignment verticalAlignment() const;
    QBindable<Qt::Alignment> bindableVerticalAlignment();
    Q_SIGNAL void verticalAlignmentChanged();

    Q_PROPERTY(QFont font READ font BINDABLE bindableFont NOTIFY fontChanged)
    QFont font() const;
    QBindable<QFont> bindableFont();
    Q_SIGNAL void fontChanged();

    void update(const std::optional<Union::TextDefinition> &borders);

private:
    Q_OBJECT_BINDABLE_PROPERTY(TextGroup, Qt::Alignment, m_horizontalAlignment, &TextGroup::horizontalAlignmentChanged)
    Q_OBJECT_BINDABLE_PROPERTY(TextGroup, Qt::Alignment, m_verticalAlignment, &TextGroup::verticalAlignmentChanged)
    Q_OBJECT_BINDABLE_PROPERTY(TextGroup, QFont, m_font, &TextGroup::fontChanged)
};

class QuickStyle : public QQuickAttachedPropertyPropagator
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Style)
    QML_ATTACHED(QuickStyle)

public:
    QuickStyle(QObject *parent = nullptr);

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

    Q_PROPERTY(TextGroup *text READ text CONSTANT)
    TextGroup *text() const;

    // Q_PROPERTY(ShadowGroup * shadow READ shadow CONSTANT)
    // ShadowGroup * shadow() const;

    // Q_PROPERTY(BackgroundGroup* background READ background CONSTANT)
    // BackgroundGroup* background() const;

    Union::ElementQuery query() const;

    Q_SIGNAL void updated();

    static QuickStyle *qmlAttachedProperties(QObject *parent);

protected:
    void attachedParentChange(QQuickAttachedPropertyPropagator *, QQuickAttachedPropertyPropagator *) override;

private:
    void setElement(QuickElement *newElement);
    void update();

    Q_OBJECT_BINDABLE_PROPERTY(QuickStyle, qreal, m_implicitWidth, &QuickStyle::implicitWidthChanged)
    Q_OBJECT_BINDABLE_PROPERTY(QuickStyle, qreal, m_implicitHeight, &QuickStyle::implicitHeightChanged)
    Q_OBJECT_BINDABLE_PROPERTY(QuickStyle, Sizes, m_padding, &QuickStyle::paddingChanged)
    Q_OBJECT_BINDABLE_PROPERTY(QuickStyle, Sizes, m_margins, &QuickStyle::marginsChanged)

    std::unique_ptr<BordersGroup> m_bordersGroup;
    std::unique_ptr<TextGroup> m_textGroup;

    QuickElement *m_element = nullptr;
};
