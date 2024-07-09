/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>

#include "union_export.h"

namespace Union
{

class Theme;

/**
 * An interface that is used by Theme to allow input plugins to load style rules.
 */
class UNION_EXPORT ThemeLoader
{
public:
    virtual ~ThemeLoader()
    {
    }

    /**
     * Load the style rules for a theme.
     */
    virtual bool load(std::shared_ptr<Theme> theme) = 0;
};

}
