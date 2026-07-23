// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <filesystem>
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
     * \property Union::Style::path
     *
     * The path where the package for this style is located.
     */
    Q_PROPERTY(std::filesystem::path path READ path CONSTANT)
    std::filesystem::path path() const;

    /*!
     * \property Union::Style::id
     *
     * The identifier of the style.
     *
     * This is effectively the filename of path().
     */
    Q_PROPERTY(QString id READ id CONSTANT)
    QString id() const;

    /*!
     * \property Union::Style::hasErrors
     *
     * Whether the style encountered any errors while loading.
     */
    Q_PROPERTY(bool hasErrors READ hasErrors WRITE setHasErrors NOTIFY hasErrorsChanged)
    bool hasErrors() const;
    void setHasErrors(bool newHasErrors);
    Q_SIGNAL void hasErrorsChanged();

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
     * Add a path to the list of files that should be considered for caching.
     */
    void addCachePath(const std::filesystem::path &path);

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
     * Create a new instance of Style from a path.
     *
     * \a path   The path to create a style from.
     * \a loader An instance of StyleLoader responsible for loading the actual
     *           data of this style.
     */
    static Ptr create(const std::filesystem::path &path, std::unique_ptr<StyleLoader> &&loader);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    friend class StyleRegistry;
    friend class StyleRegistryPrivate;

    const std::unique_ptr<StylePrivate> d;
};

class StyleChangedEvent : public QEvent
{
public:
    StyleChangedEvent();

    static QEvent::Type s_type;
};
}
