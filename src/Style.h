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

class StyleLoader;
class StylePrivate;

/*!
 * \class Union::Style
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief A collection of StyleRule instances.
 */
class UNION_EXPORT Style : public QObject, public std::enable_shared_from_this<Style>
{
    Q_OBJECT

public:
    /*!
     * \typealias Union::Style::Ptr
     *
     * Shorthand for \c{std::shared_ptr<Style>}
     */
    using Ptr = std::shared_ptr<Style>;

    Style(std::unique_ptr<StylePrivate> &&d);
    ~Style() override;

    /*!
     * \property Union::Style::name
     *
     * The name of the style.
     */
    Q_PROPERTY(QString name READ name CONSTANT)
    QString name() const;

    /*!
     * \property Union::Style::pluginName
     *
     * The name of the plugin used to load rules for this Style.
     */
    Q_PROPERTY(QString pluginName READ pluginName CONSTANT)
    QString pluginName() const;

    /*!
     * Returns the StyleLoader of this style.
     */
    StyleLoader *loader() const;

    /*!
     * Load the style.
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
     * Insert a new rule into the style.
     *
     * \a style The new rule to add.
     */
    void insert(StyleRule::Ptr style);

    /*!
     * Returns the list of all StyleRule instances contained in this style.
     */
    QList<StyleRule::Ptr> rules();

    /*!
     * Create a new instance of Style.
     *
     * \a pluginName The name of the plugin responsible for creating this style.
     * \a styleName The name of this style.
     * \a loader An instance of StyleLoader responsible for loading the actual
     *           data of this style.
     */
    static Ptr create(const QString &pluginName, const QString &styleName, std::unique_ptr<StyleLoader> &&loader);

private:
    const std::unique_ptr<StylePrivate> d;
};

}
