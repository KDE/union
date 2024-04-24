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

ElementQuery::~ElementQuery() noexcept = default;

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
        if (style->foreground().has_value() && !m_combined->foreground().has_value()) {
            m_combined->setForeground(style->foreground());
        }

        if (style->background().has_value() && !m_combined->background().has_value()) {
            m_combined->setBackground(style->background());
        }

        if (style->border().has_value() && !m_combined->border().has_value()) {
            m_combined->setBorder(style->border());
        }

        if (style->corners().has_value() && !m_combined->corners().has_value()) {
            m_combined->setCorners(style->corners());
        }

        if (style->shadow().has_value() && !m_combined->shadow().has_value()) {
            m_combined->setShadow(style->shadow());
        }

        if (style->outset().has_value() && !m_combined->outset().has_value()) {
            m_combined->setOutset(style->outset());
        }

        if (style->margins().has_value() && !m_combined->margins().has_value()) {
            m_combined->setMargins(style->margins());
        }

        if (style->padding().has_value() && !m_combined->padding().has_value()) {
            m_combined->setPadding(style->padding());
        }
    }
}

QSizeF ElementQuery::contentSize() const
{
    return m_combined->contentSize();
}

QRectF ElementQuery::boundingRect() const
{
    return m_combined->boundingRect();
}

QMarginsF ElementQuery::borderSizes() const
{
    return m_combined->borderSizes();
}

std::optional<AreaDefinition> ElementQuery::foreground() const
{
    return m_combined->foreground();
}

std::optional<AreaDefinition> ElementQuery::background() const
{
    return m_combined->background();
}

std::optional<BorderDefinition> ElementQuery::border() const
{
    return m_combined->border();
}

std::optional<CornersDefinition> ElementQuery::corners() const
{
    return m_combined->corners();
}

std::optional<ShadowDefinition> ElementQuery::shadow() const
{
    return m_combined->shadow();
}

std::optional<BorderDefinition> ElementQuery::outset() const
{
    return m_combined->outset();
}

std::optional<SizeDefinition> ElementQuery::margins() const
{
    return m_combined->margins();
}

std::optional<SizeDefinition> ElementQuery::padding() const
{
    return m_combined->padding();
}
