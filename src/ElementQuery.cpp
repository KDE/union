/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "ElementQuery.h"

#include "Theme.h"

using namespace Union;

ElementQuery::ElementQuery(std::shared_ptr<Theme> theme)
    : m_theme(theme)
{
    m_combined = Style::create();
}

QList<Element::Ptr> ElementQuery::elements()
{
    return m_elements;
}

void ElementQuery::setElements(const QList<Element::Ptr> &elements)
{
    m_elements = elements;
}

bool ElementQuery::execute()
{
    m_styles = m_theme->matches(m_elements);
    if (m_styles.isEmpty()) {
        return false;
    }

    m_combined = Style::create();

    for (auto style : std::as_const(m_styles)) {
        if (style->background().has_value() && !m_combined->background().has_value()) {
            m_combined->setBackground(style->background());
        }

        if (style->border().has_value() && !m_combined->border().has_value()) {
            m_combined->setBorder(style->border());
        }

        if (style->corners().has_value() && !m_combined->corners().has_value()) {
            m_combined->setCorners(style->corners());
        }
    }

    return true;
}

QMarginsF ElementQuery::borderSizes() const
{
    return m_combined->borderSizes();
}

std::optional<AreaDefinition> ElementQuery::background()
{
    return m_combined->background();
}

std::optional<BorderDefinition> ElementQuery::border()
{
    return m_combined->border();
}

std::optional<CornersDefinition> ElementQuery::corners()
{
    return m_combined->corners();
}
