// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QObject>

#include "union_export.h"

namespace Union
{

class Style;
/*!
 * \class Union::InputPlugin
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief Abstract base class for plugins providing input data.
 *
 * This class defines the interface input plugins should implement. Input
 * plugins are plugins providing Theme instances and other themeing specific
 * data to the system.
 */
class UNION_EXPORT InputPlugin : public QObject
{
    Q_OBJECT

public:
    InputPlugin(QObject *parent = nullptr);

    /*!
     * Create an instance of a style by name.
     *
     * This should be reimplemented by subclasses and create a new instance of
     * Style using an appropriate StyleLoader and other input specific data.
     */
    virtual std::shared_ptr<Style> createStyle(const QString &styleName) const = 0;
};

}

Q_DECLARE_INTERFACE(Union::InputPlugin, "org.kde.union.InputPlugin")
Q_DECLARE_METATYPE(Union::InputPlugin)
