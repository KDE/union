// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <memory>

#include "union_export.h"

namespace Union
{

class Style;

/*!
 * \class Union::StyleLoader
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief An interface that is used by Style to allow input plugins to load style rules.
 */
class UNION_EXPORT StyleLoader
{
public:
    virtual ~StyleLoader()
    {
    }

    /*!
     * Load the style rules for a theme.
     *
     * \a theme The theme the rules should be added to.
     */
    virtual bool load(std::shared_ptr<Style> theme) = 0;
};

}
