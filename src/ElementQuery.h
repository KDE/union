// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QList>
#include <QString>

#include "Element.h"
#include "Selector.h"
#include "StyleRule.h"

#include "union_export.h"

namespace Union
{

class Theme;
class ElementQueryPrivate;

/*!
 * \class Union::ElementQuery
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief A class encapsulating lookup of style properties based on a list of elements.
 *
 * This class will try find which style rules match a certain list of elements.
 * The list of elements is expected to represent a chain of elements from the
 * root of the application to a specific element.
 *
 * Lookup is done for individual properties, so requesting for example the
 * background color would result in checking if the first matched rule has the
 * background color set and if so, return that. If not the second rule would be
 * checked and so on.
 */
class UNION_EXPORT ElementQuery
{
public:
    /*!
     * Constructor.
     *
     * \a theme An instance of Theme that is used to get the style rules from.
     */
    explicit ElementQuery(std::shared_ptr<Theme> theme = nullptr);
    ~ElementQuery() noexcept;

    /*!
     * The list of elements that should be matched against.
     *
     * This list is treated as a hierarchy, with the first element as the bottom
     * most element and the last the top most.
     */
    QList<Element::Ptr> elements() const;

    /*!
     * Set the list of elements to match against.
     *
     * \a elements The elements to match.
     */
    void setElements(const QList<Element::Ptr> &elements);

    /*!
     * Returns the list of matched style rules for this query.
     */
    QList<StyleRule::Ptr> matchedRules() const;

    /*!
     * Execute the query.
     *
     * This will perform matching of style rules from the query's theme against
     * the list of elements in this query.
     *
     * Returns true if the query was successful, false if nothing was matched.
     */
    bool execute();

    /*!
     * Returns if the last call to execute() had any matches.
     */
    bool hasMatches() const;

    /*!
     * Returns the root of the combined matched set of properties.
     *
     * This will contain all the resolved properties of the matched style rules.
     */
    const Properties::StyleProperty &properties() const;

private:
    const std::unique_ptr<ElementQueryPrivate> d;
};

}
