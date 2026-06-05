// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QApplication>
#include <QObject>
#include <QProperty>
#include <QWidget>
#include <properties/SizePropertyGroup.h>
#include <properties/StylePropertyGroup.h>

#include "Types.h"

namespace Union
{
namespace Widgets
{

class WidgetsElement;
class QStyle;

namespace PositionerSource
{
Q_NAMESPACE

/*!
 * \enum PositionerSource::Source
 * \relates Positioner
 *
 * Indicates what property to use for alignment information.
 *
 * \value Layout
 *  Read from the Layout property.
 * \value Text
 *  Read from the Text property.
 * \value Icon
 *  Read from the Icon property.
 */
enum class Source {
    Layout,
    Text,
    Icon
};
Q_ENUM_NS(Source)

}

class PositionerLayout;

/*!
 * \qmltype Positioner
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-core
 *
 * \brief An attached property type that can be used to position various objects inside another object.
 *
 * This will position any items added to `positionItems` based on information
 * from the style.
 */
class Positioner : public QObject
{
    Q_OBJECT

public:
    using ItemList = QObjectList;

    Positioner(Union::Properties::StylePropertyGroup *properties, const QWidget *widget, QObject *parent = nullptr);
    ~Positioner() override;

    /*!
     * \qmlattachedproperty ItemList Positioner::positionItems
     *
     * A list of items that should be positioned by this positioner.
     */
    ItemList positionItems() const;
    void setPositionItems(const ItemList &newItems);
    Q_SIGNAL void positionItemsChanged();

    /*!
     * \qmlattachedproperty real Positioner::implicitWidth
     *
     * The implicit width, as calculated by the internal layout.
     */
    qreal implicitWidth() const;
    Q_SIGNAL void implicitWidthChanged();

    /*!
     * \qmlattachedproperty real Positioner::implicitHeight
     *
     * The implicit height, as calculated by the internal layout.
     */
    qreal implicitHeight() const;
    Q_SIGNAL void implicitHeightChanged();

    /*!
     * \qmlattachedproperty bool Positioner::debug
     *
     * Print debugging info for this positioner.
     */
    bool debug() const;
    void setDebug(bool newDebug);
    Q_SIGNAL void debugChanged();

    /*!
     * \qmlattachedproperty Qt::LayoutDirection Positioner::layoutDirection
     *
     * What direction the layout should use when performing the layout.
     *
     * By default, this is Qt::LayoutDirectionAuto, which means the layout will
     * use the layout direction of the application.
     */
    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection(Qt::LayoutDirection newLayoutDirection);
    Q_SIGNAL void layoutDirectionChanged();

private:
    std::pair<ItemList, ItemList> findPositionedItemChanges(const ItemList &currentItems, const ItemList &newItems);

    struct ChildrenWatcher {
        QWidget *item;
        ItemList children;
    };

    ItemList m_positionItems;
    QList<ChildrenWatcher> m_positionChildrenItems;
    PositionerLayout *m_layout;
};

/*!
 * \qmltype PositionedItem
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-core
 *
 * \brief Attached property that contains properties to control the behaviour of
 *        Positioner with regards to the attached item.
 */
class PositionedItem : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    /*!
     * \qmlattachedproperty bool PositionedItem::positionChildren
     *
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
    bool positionChildren() const;
    void setPositionChildren(bool position);

    /*!
     * \qmlattachedproperty PositionerSource::Source source
     *
     * Which property to use as source for this item's alignment.
     *
     * This can be used to indicate the item is meant to represent an icon or
     * text element of the control and those alignment properties should be used
     * for aligning this item.
     */
    PositionerSource::Source source() const;
    void setSource(PositionerSource::Source newSource);
    Q_SIGNAL void sourceChanged();

    /*!
     * \qmlattachedproperty Union::Properties::Alignment horizontalAlignment
     *
     * An override of the style-provided horizontal alignment value.
     *
     * Use this to force the alignment to a certain value, in case you want to
     * use the style's alignment for something else. For example, when you want
     * to have a text field fill the control and use the text's alignment
     * properties to control the alignment of the text inside the text field.
     */

    Union::Properties::Alignment horizontalAlignment() const;
    void setHorizontalAlignment(Union::Properties::Alignment newAlignment);
    void resetHorizontalAlignment();
    Q_SIGNAL void horizontalAlignmentChanged();

    /*!
     * \qmlattachedproperty Union::Properties::Alignment horizontalAlignment
     *
     * An override of the style-provided vertical alignment value.
     */
    Union::Properties::Alignment verticalAlignment() const;
    void setVerticalAlignment(Union::Properties::Alignment newAlignment);
    void resetVerticalAlignment();
    Q_SIGNAL void verticalAlignmentChanged();

    qreal minimumWidth() const;
    void setMinimumWidth(qreal newMinimumWidth);
    void resetMinimumWidth();
    Q_SIGNAL void minimumWidthChanged();

    qreal minimumHeight() const;
    void setMinimumHeight(qreal newMinimumHeight);
    void resetMinimumHeight();
    Q_SIGNAL void minimumHeightChanged();

protected:
    bool event(QEvent *event) override;

private:
    void sendChangedEvent();

    bool m_positionChildren = false;
    PositionerSource::Source m_source = PositionerSource::Source::Layout;
    Union::Properties::Alignment m_horizontalAlignment = Union::Properties::Alignment::Unspecified;
    Union::Properties::Alignment m_verticalAlignment = Union::Properties::Alignment::Unspecified;
    qreal m_minimumWidth = -1.0;
    qreal m_minimumHeight = -1.0;
};

class PositionedItemChangedEvent : public QEvent
{
public:
    PositionedItemChangedEvent();

    static QEvent::Type s_type;
};
}
}
