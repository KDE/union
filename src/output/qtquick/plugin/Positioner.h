/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QObject>
#include <QProperty>
#include <QQuickItem>
#include <qqmlregistration.h>

#include <PropertiesTypes.h>
#include <properties/StyleProperty.h>

#include "Types.h"
#include "properties/AlignmentPropertyGroup.h"

class QuickElement;
class QuickStyle;

namespace PositionerSource
{
Q_NAMESPACE
QML_ELEMENT

/**
 * Indicates what property to use for alignment information.
 */
enum class Source {
    Layout, ///< Read from the Layout property.
    Text, ///< Read from the Text property.
    Icon ///< Read from the Icon property.
};
Q_ENUM_NS(Source)

}

class PositionerLayout;

/**
 * An attached property that can be used to position various objects inside another object.
 *
 * This will position any items added to `positionItems` based on information
 * from the style.
 */
class PositionerAttached : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
public:
    using ItemList = QList<QQuickItem *>;

    PositionerAttached(QObject *parent = nullptr);
    ~PositionerAttached() override;

    /**
     * A list of items that should be positioned by this positioner.
     */
    Q_PROPERTY(ItemList positionItems READ positionItems WRITE setPositionItems NOTIFY positionItemsChanged)
    ItemList positionItems() const;
    void setPositionItems(const ItemList &newItems);
    Q_SIGNAL void positionItemsChanged();

    /**
     * The implicit width, as calculated by the internal layout.
     */
    Q_PROPERTY(qreal implicitWidth READ implicitWidth NOTIFY implicitWidthChanged)
    qreal implicitWidth() const;
    Q_SIGNAL void implicitWidthChanged();

    /**
     * The implicit height, as calculated by the internal layout.
     */
    Q_PROPERTY(qreal implicitHeight READ implicitHeight NOTIFY implicitHeightChanged)
    qreal implicitHeight() const;
    Q_SIGNAL void implicitHeightChanged();

    /**
     * Padding as calculated by the internal layout.
     *
     * Note that this includes the space that is reserved by the style for the
     * start and end buckets that are relative to the item container. When these
     * values are used as a Control's padding, this ensures any reserved space
     * is also excluded from the content item, making it easier to override the
     * content item without needing to specify Positioner related properties.
     */
    Q_PROPERTY(Sizes padding READ padding NOTIFY paddingChanged)
    Sizes padding() const;
    Q_SIGNAL void paddingChanged();

private:
    void onLayoutFinished();

    std::pair<ItemList, ItemList> findPositionedItemChanges(const ItemList &currentItems, const ItemList &newItems);

    struct ChildrenWatcher {
        QQuickItem *item;
        ItemList children;
    };

    ItemList m_positionItems;
    QList<ChildrenWatcher> m_positionChildrenItems;
    PositionerLayout *m_layout;
};

/**
 * An uncreatable type used to expose the Positioner attached type.
 */
class Positioner : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Just for attached properties")
    QML_ATTACHED(PositionerAttached)

public:
    using QQuickItem::QQuickItem;

    static PositionerAttached *qmlAttachedProperties(QObject *parent);
};

/**
 * Attached property that contains properties for a single item positioned by Positioner.
 */
class PositionedItemAttached : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS
public:
    using QObject::QObject;

    /**
     * Position the children of this item, not this item itself.
     *
     * This indicates the current item is merely a container for a bunch of
     * items that should themselves be positioned. Positioner will ignore this
     * item and instead track and position all its children.
     *
     * Note that this intentionally does not have a change signal, as Positioner
     * cannot handle this property changing dynamically. Instead, it is expected
     * to be set once and not change during the lifetime of the item.
     */
    Q_PROPERTY(bool positionChildren READ positionChildren WRITE setPositionChildren)
    bool positionChildren() const;
    void setPositionChildren(bool position);

    /**
     * Which property to use as source for this item's alignment.
     *
     * This can be used to indicate the item is meant to represent an icon or
     * text element of the control and those alignment properties should be used
     * for aligning this item.
     */
    Q_PROPERTY(PositionerSource::Source source READ source WRITE setSource NOTIFY sourceChanged)
    PositionerSource::Source source() const;
    void setSource(PositionerSource::Source newSource);
    Q_SIGNAL void sourceChanged();

    /**
     * An override of the style-provided horizontal alignment value.
     *
     * Use this to force the alignment to a certain value, in case you want to
     * use the style's alignment for something else. For example, when you want
     * to have a text field fill the control and use the text's alignment
     * properties to control the alignment of the text inside the text field.
     */
    Q_PROPERTY(Union::Properties::Alignment horizontalAlignment READ horizontalAlignment WRITE setHorizontalAlignment RESET resetHorizontalAlignment //
                   NOTIFY horizontalAlignmentChanged)
    Union::Properties::Alignment horizontalAlignment() const;
    void setHorizontalAlignment(Union::Properties::Alignment newAlignment);
    void resetHorizontalAlignment();
    Q_SIGNAL void horizontalAlignmentChanged();

    /**
     * An override of the style-provided vertical alignment value.
     */
    Q_PROPERTY(Union::Properties::Alignment verticalAlignment READ verticalAlignment WRITE setVerticalAlignment RESET resetVerticalAlignment //
                   NOTIFY verticalAlignmentChanged)
    Union::Properties::Alignment verticalAlignment() const;
    void setVerticalAlignment(Union::Properties::Alignment newAlignment);
    void resetVerticalAlignment();
    Q_SIGNAL void verticalAlignmentChanged();

private:
    bool m_positionChildren = false;
    PositionerSource::Source m_source = PositionerSource::Source::Layout;
    Union::Properties::Alignment m_horizontalAlignment = Union::Properties::Alignment::Unspecified;
    Union::Properties::Alignment m_verticalAlignment = Union::Properties::Alignment::Unspecified;
};

/**
 * Uncreatable item to expose PositionedItem as attached object to QML.
 */
class PositionedItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Just for attached properties")
    QML_ATTACHED(PositionedItemAttached)

public:
    using QQuickItem::QQuickItem;

    static PositionedItemAttached *qmlAttachedProperties(QObject *parent);
};
