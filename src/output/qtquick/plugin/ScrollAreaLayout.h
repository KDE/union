// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Artem Grinev <agrinev98@gmail.com>

#pragma once

#include <memory>

#include <QQuickItem>
#include <qqmlregistration.h>

namespace Union
{
namespace Quick
{

/*!
 * \qmltype ScrollAreaLayout
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-core
 *
 * \brief Determines which scroll bars a scroll area needs to show.
 *
 * Whether a scroll bar is needed depends on the space available to the contents
 * of a scroll area, while showing one reduces that space, so as bindings this
 * ends up depending on itself. Doing it during layout instead allows simply
 * repeating the calculation until it no longer changes, which is what
 * `updatePolish()` is used for here: items polished during polishing are
 * handled in the same pass, so a repeat still makes it into the same frame.
 *
 * The results are exposed as properties rather than applied to the scroll bars,
 * so anything assigning to those keeps overriding this as usual.
 *
 * Expected to be created as a child of the scroll area it applies to.
 */
class ScrollAreaLayout : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    ScrollAreaLayout(QQuickItem *parent = nullptr);
    ~ScrollAreaLayout() override;

    Q_SLOT void markDirty();

    /*!
     * \qmlproperty Item ScrollAreaLayout::flickable
     *
     * The Flickable containing the contents, whose content size determines
     * whether scroll bars are needed.
     */
    Q_PROPERTY(QQuickItem *flickable READ flickable WRITE setFlickable NOTIFY flickableChanged)
    QQuickItem *flickable() const;
    void setFlickable(QQuickItem *newFlickable);
    Q_SIGNAL void flickableChanged();

    /*!
     * \qmlproperty Item ScrollAreaLayout::verticalScrollBar
     *
     * The vertical scroll bar of the scroll area, if any.
     */
    Q_PROPERTY(QQuickItem *verticalScrollBar READ verticalScrollBar WRITE setVerticalScrollBar NOTIFY verticalScrollBarChanged)
    QQuickItem *verticalScrollBar() const;
    void setVerticalScrollBar(QQuickItem *newScrollBar);
    Q_SIGNAL void verticalScrollBarChanged();

    /*!
     * \qmlproperty Item ScrollAreaLayout::horizontalScrollBar
     *
     * The horizontal scroll bar of the scroll area, if any.
     */
    Q_PROPERTY(QQuickItem *horizontalScrollBar READ horizontalScrollBar WRITE setHorizontalScrollBar NOTIFY horizontalScrollBarChanged)
    QQuickItem *horizontalScrollBar() const;
    void setHorizontalScrollBar(QQuickItem *newScrollBar);
    Q_SIGNAL void horizontalScrollBarChanged();

    /*!
     * \qmlproperty bool ScrollAreaLayout::verticalVisible
     *
     * Whether the vertical scroll bar should be shown.
     */
    Q_PROPERTY(bool verticalVisible READ verticalVisible NOTIFY scrollBarVisibilityChanged)
    bool verticalVisible() const;

    /*!
     * \qmlproperty bool ScrollAreaLayout::horizontalVisible
     *
     * Whether the horizontal scroll bar should be shown.
     */
    Q_PROPERTY(bool horizontalVisible READ horizontalVisible NOTIFY scrollBarVisibilityChanged)
    bool horizontalVisible() const;

    Q_SIGNAL void scrollBarVisibilityChanged();

    /*!
     * \qmlproperty real ScrollAreaLayout::reservedWidth
     *
     * The space to reserve for the vertical scroll bar, or 0 if it is not shown.
     */
    Q_PROPERTY(qreal reservedWidth READ reservedWidth NOTIFY reservedSizeChanged)
    qreal reservedWidth() const;

    /*!
     * \qmlproperty real ScrollAreaLayout::reservedHeight
     *
     * The space to reserve for the horizontal scroll bar, or 0 if it is not shown.
     */
    Q_PROPERTY(qreal reservedHeight READ reservedHeight NOTIFY reservedSizeChanged)
    qreal reservedHeight() const;

    Q_SIGNAL void reservedSizeChanged();

protected:
    bool eventFilter(QObject *target, QEvent *event) override;
    void itemChange(ItemChange change, const ItemChangeData &data) override;
    void updatePolish() override;

private:
    void connectScrollBar(QQuickItem *scrollBar);
    void disconnectItem(QQuickItem *item);

    class Private;
    const std::unique_ptr<Private> d;
};

}
}
