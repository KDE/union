// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include <Element.h>
#include <properties/SizePropertyGroup.h>
#include <properties/StylePropertyGroup.h>

#include <QMargins>
#include <QPainterPath>

class QStyleOption;

struct LayoutItem {
    QString elementName;
    int order;
    Union::Properties::Alignment horizontalAlignment;
    Union::Properties::Alignment verticalAlignment;
    QRectF rect;
};

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

Union::ElementList prepareElements(const QStyleOption *opt, const QWidget *widget = nullptr, QStringList childElementNames = {});
Union::Properties::StylePropertyGroup *queryProperties(const Union::ElementList &elements);

QStringList setupMemberList(const QWidget *widget);

QMap<QString, LayoutItem> layoutMap(const Union::ElementList &elements, const QStyleOption *opt, const QStringList &subElements);

QString textFromOption(const QStyleOption *opt);

int textFlagsFromProperties(Union::Properties::StylePropertyGroup *properties, bool skipAlign);

QRect centerRect(const QRect &rect, int width, int height);