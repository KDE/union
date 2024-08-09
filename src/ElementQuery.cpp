/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "ElementQuery.h"

#include "Theme.h"

#include "union_query_logging.h"

using namespace Union;

class Union::ElementQueryPrivate
{
public:
    std::shared_ptr<Theme> theme;
    QList<Element::Ptr> elements;
    QList<StyleRule::Ptr> styles;
    std::optional<Properties::StyleProperty> properties = std::nullopt;

    StyleRule::Ptr combined;

    bool result = false;
    inline static const Properties::StyleProperty emptyProperties;
};

ElementQuery::ElementQuery(std::shared_ptr<Theme> theme)
    : d(std::make_unique<ElementQueryPrivate>())
{
    d->theme = theme;
    d->combined = StyleRule::create();
}

ElementQuery::~ElementQuery() noexcept = default;

QList<Element::Ptr> ElementQuery::elements()
{
    return d->elements;
}

void ElementQuery::setElements(const QList<Element::Ptr> &elements)
{
    d->elements = elements;
}

void ElementQuery::execute()
{
    qCDebug(UNION_QUERY) << "Trying to match" << d->elements;

    d->styles = d->theme->matches(d->elements);
    d->combined = StyleRule::create();

    if (d->styles.isEmpty()) {
        qCDebug(UNION_QUERY) << "Did not match any style rules!";
        d->combined = StyleRule::create();
        d->result = false;
        return;
    }

    qCDebug(UNION_QUERY) << "Matched style rules:";
    qCDebug(UNION_QUERY) << d->styles;

    d->combined = StyleRule::create();
    d->result = true;

    for (auto style : std::as_const(d->styles)) {
        if (style->foreground().has_value() && !d->combined->foreground().has_value()) {
            d->combined->setForeground(style->foreground());
        }

        if (style->background().has_value() && !d->combined->background().has_value()) {
            d->combined->setBackground(style->background());
        }

        if (style->border().has_value() && !d->combined->border().has_value()) {
            d->combined->setBorder(style->border());
        }

        if (style->corners().has_value() && !d->combined->corners().has_value()) {
            d->combined->setCorners(style->corners());
        }

        if (style->shadow().has_value() && !d->combined->shadow().has_value()) {
            d->combined->setShadow(style->shadow());
        }

        if (style->outset().has_value() && !d->combined->outset().has_value()) {
            d->combined->setOutset(style->outset());
        }

        if (style->margins().has_value() && !d->combined->margins().has_value()) {
            d->combined->setMargins(style->margins());
        }

        if (style->padding().has_value() && !d->combined->padding().has_value()) {
            d->combined->setPadding(style->padding());
        }

        if (style->text().has_value() && !d->combined->text().has_value()) {
            d->combined->setText(style->text());
        }

        if (style->icon().has_value() && !d->combined->icon().has_value()) {
            d->combined->setIcon(style->icon());
        }
    }
}

bool ElementQuery::result() const
{
    return d->result;
}

QSizeF ElementQuery::contentSize() const
{
    return d->combined->contentSize();
}

QRectF ElementQuery::boundingRect() const
{
    return d->combined->boundingRect();
}

QMarginsF ElementQuery::borderSizes() const
{
    return d->combined->borderSizes();
}

std::optional<AreaDefinition> ElementQuery::foreground() const
{
    return d->combined->foreground();
}

std::optional<AreaDefinition> ElementQuery::background() const
{
    return d->combined->background();
}

std::optional<BorderDefinition> ElementQuery::border() const
{
    return d->combined->border();
}

std::optional<CornersDefinition> ElementQuery::corners() const
{
    return d->combined->corners();
}

std::optional<ShadowDefinition> ElementQuery::shadow() const
{
    return d->combined->shadow();
}

std::optional<BorderDefinition> ElementQuery::outset() const
{
    return d->combined->outset();
}

std::optional<SizeDefinition> ElementQuery::margins() const
{
    return d->combined->margins();
}

std::optional<SizeDefinition> ElementQuery::padding() const
{
    return d->combined->padding();
}

std::optional<TextDefinition> ElementQuery::text() const
{
    return d->combined->text();
}

std::optional<IconDefinition> ElementQuery::icon() const
{
    return d->combined->icon();
}

const Properties::StyleProperty &ElementQuery::properties() const
{
    return d->properties.has_value() ? d->properties.value() : d->emptyProperties;
}
