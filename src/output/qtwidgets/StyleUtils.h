// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include <Element.h>
#include <properties/SizePropertyGroup.h>
#include <properties/StylePropertyGroup.h>

#include <QMargins>
#include <QPainterPath>

class QStyleOption;

/*!
 * \brief Translate the state from QStyleOption to Union::Element states.
 */
Union::Element::States statesFromOption(const QStyleOption *option);
QStringList hintsFromOption(const QStyleOption *option);
Union::Element::ColorSet colorsetFromOption(const QStyleOption *option);
QVariantMap attributesFromOption(const QStyleOption *option);

QRectF prepareRectangle(const QStyleOption *option, const Union::Properties::StylePropertyGroup *properties, const QMarginsF &adjustments = QMarginsF());

Union::Properties::StylePropertyGroup *prepareProperties(Union::Element::Ptr &element);
