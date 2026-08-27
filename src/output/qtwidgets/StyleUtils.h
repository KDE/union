// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include "qtwidgets_logging.h"
#include <Element.h>
#include <properties/SizePropertyGroup.h>
#include <properties/StylePropertyGroup.h>

#include <QMargins>
#include <QPainterPath>
#include <QStyleOption>

class QStyleOption;

struct LayoutItem {
    QString elementName;
    int order;
    Union::Properties::Alignment horizontalAlignment;
    Union::Properties::Alignment verticalAlignment;
    QRectF rect;
};

const char property_union_member_list[] = "_union_member_list";

Qt::Alignment toQtAlignment(Union::Properties::AlignmentPropertyGroup *alignmentGroup);
Qt::TextElideMode toQtElideMode(Union::Properties::TextElide elideMode);
Qt::TextFlag toQtWrapMode(Union::Properties::TextWrapMode wrapMode);

/*!
 * \brief Returns the background rectangle of an option, but removes its insets according to the properties first.
 */
QRectF backgroundRectangle(const QStyleOption *option, const Union::Properties::StylePropertyGroup *properties);

/*!
 * \brief Matches the widget name/class to a matching CSS element name, and sets up
 * property "_union_member_list" to the widget. This can be used to get the whole parental
 * hierarchy of the widget
 */
QStringList widgetToElementHierarchy(const QWidget *widget);

/*!
 * \brief Helper function to get text from any QStyleOption that has a field with QString (text/title)
 */
QString textFromOption(const QStyleOption *opt);

/*!
 * \brief Returns flags for text drawing purposes. If using LayoutMap for alignment, it's best to
 * skip the Qt alignment by setting skipAlign to true.
 */
int textFlagsFromProperties(Union::Properties::StylePropertyGroup *properties, bool skipAlign);

/*!
 * \brief Centers a rectangle depending on width and height. Copied from Breeze.
 */
QRectF centerRect(const QRectF &rect, int width, int height);

/*!
 * \brief Tries to match styleOption type to a potential element.
 * Used when widget is null.
 */
QString styleOptionToElementName(const QStyleOption *option);

// Calculates the bounding box rectangle from items inside layoutMap
QRectF unifiedRect(QMap<QString, LayoutItem> layoutMap);

// Shared hints for frameOptions
QStringList frameHints(const QStyleOptionFrame *frameOption);
