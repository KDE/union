// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include <Element.h>
#include <properties/SizeProperty.h>
#include <properties/StyleProperty.h>

#include <QMargins>
#include <QPainterPath>

class QStyleOption;

/*!
 * \brief Translate the state from QStyleOption to Union::Element states.
 */
Union::Element::States statesFromOption(const QStyleOption *option);
