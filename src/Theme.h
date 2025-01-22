// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <memory>

#include <QObject>

#include "Element.h"
#include "StyleRule.h"

#include "union_export.h"

namespace Union
{

class ThemeLoader;
class ThemePrivate;

/*!
 * \class Union::Theme
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief A collection of StyleRule instances.
 */
class UNION_EXPORT Theme : public QObject, public std::enable_shared_from_this<Theme>
{
    Q_OBJECT

public:
    /*!
     * \typealias Union::Theme::Ptr
     *
     * Shorthand for \c{std::shared_ptr<Theme>}
     */
    using Ptr = std::shared_ptr<Theme>;

    Theme(std::unique_ptr<ThemePrivate> &&d);
    ~Theme() override;

    /*!
     * \property Union::Theme::name
     *
     * The name of the theme.
     */
    Q_PROPERTY(QString name READ name CONSTANT)
    QString name() const;

    /*!
     * \property Union::Theme::pluginName
     *
     * The name of the plugin used to load rules for this Theme.
     */
    Q_PROPERTY(QString pluginName READ pluginName CONSTANT)
    QString pluginName() const;

    /*!
     * Returns the ThemeLoader of this theme.
     */
    ThemeLoader *loader() const;

    /*!
     * Load the theme.
     *
     * Returns true on success or false if an error occurred.
     */
    bool load();

    /*!
     * Returns the list of StyleRule instances that matches a given list of
     * Element instances.
     */
    QList<StyleRule::Ptr> matches(const QList<Element::Ptr> &elements);

    /*!
     * Insert a new rule into the theme.
     *
     * \a style The new rule to add.
     */
    void insert(StyleRule::Ptr style);

    /*!
     * Returns the list of all StyleRule instances contained in this theme.
     */
    QList<StyleRule::Ptr> rules();

    /*!
     * Create a new instance of Theme.
     *
     * \a pluginName The name of the plugin responsible for creating this theme.
     * \a themeName The name of this theme.
     * \a loader An instance of ThemeLoader responsible for loading the actual
     *           data of this theme.
     */
    static Ptr create(const QString &pluginName, const QString &themeName, std::unique_ptr<ThemeLoader> &&loader);

private:
    const std::unique_ptr<ThemePrivate> d;
};

}
