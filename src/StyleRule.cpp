/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "StyleRule.h"

#include <QHash>

using namespace Union;

class Union::StyleRulePrivate
{
public:
    SelectorList selectors;

    std::optional<AreaDefinition> foreground;
    std::optional<TextDefinition> text;
    std::optional<IconDefinition> icon;
    std::optional<AreaDefinition> background;
    std::optional<BorderDefinition> border;
    std::optional<ShadowDefinition> shadow;
    std::optional<SizeDefinition> margins;
    std::optional<SizeDefinition> padding;
    std::optional<BorderDefinition> outset;
    std::optional<CornersDefinition> corners;
    std::optional<LayoutDefinition> layout;
};

StyleRule::StyleRule(std::unique_ptr<StyleRulePrivate> &&d)
    : QObject()
    , d(std::move(d))
{
}

StyleRule::~StyleRule() = default;

SelectorList StyleRule::selectors() const
{
    return d->selectors;
}

void StyleRule::setSelectors(const SelectorList &selectors)
{
    d->selectors = selectors;
}

QSizeF StyleRule::contentSize() const
{
    const auto f = d->foreground.value_or(AreaDefinition{});
    const auto b = d->background.value_or(AreaDefinition{});

    return QSizeF{std::max(f.size.width(), b.size.width()), std::max(f.size.height(), b.size.height())};
}

QMarginsF StyleRule::borderSizes() const
{
    if (!border().has_value()) {
        return QMarginsF{};
    }

    const auto b = border().value();

    QMarginsF result;
    result.setLeft(b.left.value_or(LineDefinition{}).size);
    result.setRight(b.right.value_or(LineDefinition{}).size);
    result.setTop(b.top.value_or(LineDefinition{}).size);
    result.setBottom(b.bottom.value_or(LineDefinition{}).size);

    return result;
}

QRectF StyleRule::boundingRect() const
{
    QRectF result;

    auto c = contentSize();
    auto b = borderSizes();

    if (!c.isValid() && b.isNull()) {
        return QRectF{};
    }

    result.setWidth(b.left() + c.width() + b.right());
    result.setHeight(b.top() + c.height() + b.bottom());

    return result;
}

std::optional<AreaDefinition> StyleRule::foreground() const
{
    return d->foreground;
}

void StyleRule::setForeground(const std::optional<AreaDefinition> &newForeground)
{
    d->foreground = newForeground;
}

std::optional<AreaDefinition> StyleRule::background() const
{
    return d->background;
}

void StyleRule::setBackground(const std::optional<AreaDefinition> &newBackground)
{
    d->background = newBackground;
}

std::optional<BorderDefinition> StyleRule::border() const
{
    return d->border;
}

void StyleRule::setBorder(const std::optional<BorderDefinition> &newBorder)
{
    d->border = newBorder;
}

std::optional<ShadowDefinition> StyleRule::shadow() const
{
    return d->shadow;
}

void StyleRule::setShadow(const std::optional<ShadowDefinition> &newShadow)
{
    d->shadow = newShadow;
}

std::optional<SizeDefinition> StyleRule::margins() const
{
    return d->margins;
}

void StyleRule::setMargins(const std::optional<SizeDefinition> &newMargins)
{
    d->margins = newMargins;
}

std::optional<SizeDefinition> StyleRule::padding() const
{
    return d->padding;
}

void StyleRule::setPadding(const std::optional<SizeDefinition> &newPadding)
{
    d->padding = newPadding;
}

std::optional<BorderDefinition> StyleRule::outset() const
{
    return d->outset;
}

void StyleRule::setOutset(const std::optional<BorderDefinition> &newOutset)
{
    d->outset = newOutset;
}

std::optional<CornersDefinition> StyleRule::corners() const
{
    return d->corners;
}

void StyleRule::setCorners(const std::optional<CornersDefinition> &newCorners)
{
    d->corners = newCorners;
}

std::optional<TextDefinition> Union::StyleRule::text() const
{
    return d->text;
}

void Union::StyleRule::setText(const std::optional<TextDefinition> &newText)
{
    d->text = newText;
}

std::optional<IconDefinition> Union::StyleRule::icon() const
{
    return d->icon;
}

void Union::StyleRule::setIcon(const std::optional<IconDefinition> &newIcon)
{
    d->icon = newIcon;
}

StyleRule::Ptr StyleRule::create()
{
    return std::make_shared<StyleRule>(std::make_unique<StyleRulePrivate>());
}

QDebug operator<<(QDebug debug, std::shared_ptr<Union::StyleRule> style)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "StyleRule(" << style->selectors() << ")";
    return debug;
}
