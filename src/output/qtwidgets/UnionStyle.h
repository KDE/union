// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include "LruCache.h"
#include <Element.h>
#include <Style.h>

#include "elements/AbstractElement.h"
#include "elements/CheckBoxElement.h"
#include <QCommonStyle>

/*!
 * \brief Provides Union styling for QtWidgets applications.
 */
class UnionStyle : public QCommonStyle
{
    Q_OBJECT
    /* KStyle has custom elements mechanism, disable it. */
    Q_CLASSINFO("X-KDE-CustomElements", "false")

public:
    UnionStyle();
    ~UnionStyle() override;

    void drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    SubControl hitTestComplexControl(ComplexControl, const QStyleOptionComplex *, const QPoint &, const QWidget *) const override;
    void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;

    QSize sizeFromContents(QStyle::ContentsType contentsType, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const override;
    QRect subElementRect(QStyle::SubElement element, const QStyleOption *option, const QWidget *widget = nullptr) const override;
    QRect
    subControlRect(ComplexControl complexControl, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget = nullptr) const override;

    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override;
    int styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const override;

    QIcon standardIcon(StandardPixmap pixmap, const QStyleOption *option = nullptr, const QWidget *widget = nullptr) const override;

    void polish(QApplication *application) override;
    void polish(QWidget *) override;

    bool eventFilter(QObject *object, QEvent *event) override;

    // QStyle::visualAlignment, but constexpr
    static constexpr Qt::Alignment visualAlignment(Qt::LayoutDirection direction, Qt::Alignment alignment, Qt::Alignment defaultHorizontalAlignment = Qt::AlignLeft)
    {
        if (!alignment.testAnyFlags(Qt::AlignHorizontal_Mask)) {
            alignment |= defaultHorizontalAlignment & Qt::AlignHorizontal_Mask;
        }
        if (alignment.testFlag(Qt::AlignAbsolute)) {
            return alignment;
        }
        constexpr Qt::Alignment leftRightMask = Qt::AlignLeft | Qt::AlignRight;
        if (direction == Qt::RightToLeft && alignment.testAnyFlags(leftRightMask)) {
            alignment ^= leftRightMask;
        }
        return alignment | Qt::AlignAbsolute;
    }

    // QStyle::visualRect, but constexpr and using QRectF
    static constexpr QRectF visualRect(Qt::LayoutDirection direction, const QRectF &boundingRect, const QRectF &logicalRect)
    {
        if (direction == Qt::LeftToRight) {
            return logicalRect;
        }
        return {boundingRect.x() + boundingRect.right() - logicalRect.right(), logicalRect.y(), logicalRect.width(), logicalRect.height()};
    }

    // QStyle::visualPos, but constexpr and using QRectF/QPointF
    static constexpr QPointF visualPos(Qt::LayoutDirection direction, const QRectF &boundingRect, const QPointF &logicalPos)
    {
        if (direction == Qt::LeftToRight) {
            return logicalPos;
        }
        return {boundingRect.right() - logicalPos.x(), logicalPos.y()};
    }

    // QStyle::alignedRect, but constexpr, no direction parameter and using QSizeF/QRectF.
    // Unlike QStyle::alignedRect, you need to do RTL alignment adjustments with
    // visualAlignment outside of this function.
    static constexpr QRectF alignedRect(Qt::Alignment alignment, const QSizeF &contentSize, const QRectF &containerRect)
    {
        QRectF contentRect{containerRect.topLeft(), contentSize};
        if (alignment.testFlag(Qt::AlignVCenter)) {
            contentRect.moveTop(containerRect.y() + (containerRect.height() - contentRect.height()) / 2);
        } else if (alignment.testFlag(Qt::AlignBottom)) {
            contentRect.moveBottom(containerRect.bottom());
        }
        if (alignment.testFlag(Qt::AlignRight)) {
            contentRect.moveRight(containerRect.right());
        } else if (alignment.testFlag(Qt::AlignHCenter)) {
            contentRect.moveLeft(containerRect.x() + (containerRect.width() - contentRect.width()) / 2);
        }
        return contentRect;
    }

    // QStyle::alignedRect, but constexpr and using QSizeF/QRectF
    // A mostly drop-in replacement for QStyle::alignedRect.
    static constexpr QRectF alignedRect(Qt::LayoutDirection direction, Qt::Alignment alignment, const QSizeF &contentSize, const QRectF &containerRect)
    {
        alignment = visualAlignment(direction, alignment);
        return alignedRect(alignment, contentSize, containerRect);
    }

    // QStyle::drawItemPixmap, but using QRectF.
    // Not an override of QStyle::drawItemPixmap.
    // Unlike QStyle::drawItemPixmap, you need to do RTL alignment adjustments with
    // visualAlignment outside of this function.
    void drawItemPixmap(QPainter *painter, const QRectF &rect, Qt::Alignment alignment, const QPixmap &pixmap) const;

    void drawItemPixmap(QPainter *painter, const QRect &rect, int alignment, const QPixmap &pixmap) const override;

    void drawItemText(QPainter *painter,
                      const QRect &rect,
                      int flags,
                      const QPalette &pal,
                      bool enabled,
                      const QString &text,
                      QPalette::ColorRole textRole = QPalette::NoRole) const override;

    QIcon unionIcon(Union::Properties::StylePropertyGroup *properties, const QString &defaultName) const;

    /*!
     * \brief Matches the widget name/class to a matching CSS element name, and sets up
     * property "_union_member_list" to the widget. This can be used to get the whole parental
     * hierarchy of the widget
     */
    QStringList widgetToElementHierarchy(const QWidget *widget) const;

private:
    bool m_showMnemonics;
    void setMnemonics(bool enabled);
};
