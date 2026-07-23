// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include "PluginRegistry.h"
#include "StylePackage.h"

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
class UNION_EXPORT InputPlugin : public Plugin
{
    Q_OBJECT

public:
    InputPlugin(QObject *parent = nullptr);

    /*!
     * Validate if a given package is a valid package for this input.
     *
     * This should be reimplemented by subclasses and check whether the given
     * package has all files required for createStyle() to work correctly.
     */
    virtual StylePackage::Error validatePackage(const StylePackage &package) = 0;

    /*!
     * Create an instance of a style from a package.
     *
     * This should be reimplemented by subclasses and create a new instance of
     * Style using an appropriate StyleLoader and other input specific data.
     */
    virtual std::shared_ptr<Style> createStyle(const StylePackage &package) const = 0;

    /*!
     * Returns an input plugin by type name.
     */
    static std::shared_ptr<InputPlugin> inputPlugin(const QString &typeName);

    /*!
     * Add an input type manually.
     */
    static void addInputType(const QString &typeName, const std::shared_ptr<InputPlugin> &plugin);
};

}

Q_DECLARE_INTERFACE(Union::InputPlugin, "org.kde.union.InputPlugin")
Q_DECLARE_METATYPE(Union::InputPlugin)
