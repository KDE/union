// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include <Element.h>
#include <properties/SizePropertyGroup.h>
#include <properties/StylePropertyGroup.h>

#include <QMargins>
#include <QPainterPath>

class QStyleOption;

const char property_union_member_list[] = "_union_member_list";

/*!
 * \brief Translate the state from QStyleOption to Union::Element states.
 */
Union::Element::States statesFromOption(const QStyleOption *option);
QStringList hintsFromOption(const QStyleOption *option);
Union::Element::ColorSet colorsetFromOption(const QStyleOption *option);
QVariantMap attributesFromOption(const QStyleOption *option);

Qt::Alignment toQtAlignment(Union::Properties::AlignmentPropertyGroup *alignmentGroup);
Qt::TextElideMode toQtElideMode(Union::Properties::TextElide elideMode);
Qt::TextFlag toQtWrapMode(Union::Properties::TextWrapMode wrapMode);

QRectF backgroundRectangle(const QStyleOption *option, const Union::Properties::StylePropertyGroup *properties);

Union::Properties::StylePropertyGroup *prepareProperties(Union::Element::Ptr &element);

QStringList setupMemberList(QWidget *widget);

QMap<QString, QRectF> layoutMap(const QRect &mainRect, const QStyleOption *opt, const QList<Union::Element::Ptr> &elementList, const QStringList &subElements);