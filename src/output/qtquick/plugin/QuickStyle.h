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

/**
 * A grouped property for style properties related to borders.
 */
class BordersGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    BordersGroup();

    /**
     * The sizes of the borders.
     */
    Q_PROPERTY(Sizes sizes READ sizes BINDABLE bindableSizes NOTIFY sizesChanged)
    Sizes sizes() const;
    QBindable<Sizes> bindableSizes();
    Q_SIGNAL void sizesChanged();

    void update(const std::optional<Union::BorderDefinition> &borders);

private:
    Q_OBJECT_BINDABLE_PROPERTY(BordersGroup, Sizes, m_sizes, &BordersGroup::sizesChanged)
};

/**
 * A grouped property for style properties related to text.
 */
class TextGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    TextGroup();

    /**
     * Horizontal alignment for text.
     */
    Q_PROPERTY(Qt::Alignment horizontalAlignment READ horizontalAlignment BINDABLE bindableHorizontalAlignment NOTIFY horizontalAlignmentChanged)
    Qt::Alignment horizontalAlignment() const;
    QBindable<Qt::Alignment> bindableHorizontalAlignment();
    Q_SIGNAL void horizontalAlignmentChanged();

    /**
     * Vertical alignment for text.
     */
    Q_PROPERTY(Qt::Alignment verticalAlignment READ verticalAlignment BINDABLE bindableVerticalAlignment NOTIFY verticalAlignmentChanged)
    Qt::Alignment verticalAlignment() const;
    QBindable<Qt::Alignment> bindableVerticalAlignment();
    Q_SIGNAL void verticalAlignmentChanged();

    /**
     * The font to use for text.
     */
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

/**
 * A grouped property for style properties related to icons.
 */
class IconGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    IconGroup();

    /**
     * Color for icon.
     */
    Q_PROPERTY(QColor color READ color BINDABLE bindableColor NOTIFY colorChanged)
    QColor color() const;
    QBindable<QColor> bindableColor();
    Q_SIGNAL void colorChanged();

    /**
     * Width for icon.
     */
    Q_PROPERTY(qreal width READ width BINDABLE bindableWidth NOTIFY widthChanged)
    qreal width() const;
    QBindable<qreal> bindableWidth();
    Q_SIGNAL void widthChanged();

    /**
     * Height for icon.
     */
    Q_PROPERTY(qreal height READ height BINDABLE bindableHeight NOTIFY heightChanged)
    qreal height() const;
    QBindable<qreal> bindableHeight();
    Q_SIGNAL void heightChanged();

    /**
     * Icon name from an icon theme.
     */
    Q_PROPERTY(QString name READ name BINDABLE bindableName NOTIFY nameChanged)
    QString name() const;
    QBindable<QString> bindableName();
    Q_SIGNAL void nameChanged();

    /**
     * Source URL for icon.
     */
    Q_PROPERTY(QUrl source READ source BINDABLE bindableSource NOTIFY sourceChanged)
    QUrl source() const;
    QBindable<QUrl> bindableSource();
    Q_SIGNAL void sourceChanged();

    void update(const std::optional<Union::IconDefinition> &icon);

private:
    Q_OBJECT_BINDABLE_PROPERTY(IconGroup, QColor, m_color, &IconGroup::colorChanged)
    Q_OBJECT_BINDABLE_PROPERTY(IconGroup, qreal, m_width, &IconGroup::widthChanged)
    Q_OBJECT_BINDABLE_PROPERTY(IconGroup, qreal, m_height, &IconGroup::heightChanged)
    Q_OBJECT_BINDABLE_PROPERTY(IconGroup, QString, m_name, &IconGroup::nameChanged)
    Q_OBJECT_BINDABLE_PROPERTY(IconGroup, QUrl, m_source, &IconGroup::sourceChanged)
};

/**
 * An attached property that exposes style properties.
 *
 * This can be used to access style properties relevant to the current element
 * or child. It will try to find the closest QuickElement instance, either the
 * one attached to the same Item as this class is attached to, or to any parent
 * Item.
 */
class QuickStyle : public QQuickAttachedPropertyPropagator
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Style)
    QML_ATTACHED(QuickStyle)

public:
    QuickStyle(QObject *parent = nullptr);

    /**
     * The implicit width from the style.
     *
     * This exposes the width of the bounding box from the style rules. See
     * Union::Style::boundingBox() for how that is calculated.
     */
    Q_PROPERTY(qreal implicitWidth READ implicitWidth NOTIFY implicitWidthChanged BINDABLE bindableImplicitWidth)
    qreal implicitWidth() const;
    QBindable<qreal> bindableImplicitWidth();
    Q_SIGNAL void implicitWidthChanged();

    /**
     * The implicit height from the style.
     *
     * This exposes the height of the bounding box from the style rules. See
     * Union::Style::boundingBox() for how that is calculated.
     */
    Q_PROPERTY(qreal implicitHeight READ implicitHeight NOTIFY implicitHeightChanged BINDABLE bindableImplicitHeight)
    qreal implicitHeight() const;
    QBindable<qreal> bindableImplicitHeight();
    Q_SIGNAL void implicitHeightChanged();

    /**
     * The padding values from the style.
     */
    Q_PROPERTY(Sizes padding READ padding NOTIFY paddingChanged BINDABLE bindablePadding)
    Sizes padding() const;
    QBindable<Sizes> bindablePadding();
    Q_SIGNAL void paddingChanged();

    /**
     * The margin values from the style.
     */
    Q_PROPERTY(Sizes margins READ margins NOTIFY marginsChanged BINDABLE bindableMargins)
    Sizes margins() const;
    QBindable<Sizes> bindableMargins();
    Q_SIGNAL void marginsChanged();

    /**
     * A grouped property to access border properties.
     */
    Q_PROPERTY(BordersGroup *borders READ borders CONSTANT)
    BordersGroup *borders() const;

    /**
     * A grouped property to access text properties.
     */
    Q_PROPERTY(TextGroup *text READ text CONSTANT)
    TextGroup *text() const;

    /**
     * A grouped property to access icon properties.
     */
    Q_PROPERTY(IconGroup *icon READ icon CONSTANT)
    IconGroup *icon() const;

    // Q_PROPERTY(ShadowGroup * shadow READ shadow CONSTANT)
    // ShadowGroup * shadow() const;

    // Q_PROPERTY(BackgroundGroup* background READ background CONSTANT)
    // BackgroundGroup* background() const;

    /**
     * The ElementQuery associated with this instance.
     *
     * This exposes the query from the QuickElement that this style is linked
     * to.
     */
    Union::ElementQuery *query() const;

    /**
     * Emitted whenever something in the underlying style rule selection changes.
     *
     * Most importantly, this will be emitted when the matched style rules
     * change due to a change in elements.
     */
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
    std::unique_ptr<IconGroup> m_iconGroup;

    QuickElement *m_element = nullptr;
};
