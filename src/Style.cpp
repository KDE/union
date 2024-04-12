/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "Style.h"

#include <QHash>

using namespace Union;

class Union::StylePrivate
{
public:
    SelectorList selectors;

    std::optional<AreaDefinition> foreground;
    std::optional<AreaDefinition> background;
    std::optional<BorderDefinition> border;
    std::optional<ShadowDefinition> shadow;
    std::optional<SizeDefinition> margin;
    std::optional<SizeDefinition> padding;
    std::optional<BorderDefinition> outset;
    std::optional<CornersDefinition> corners;
};

Style::Style(std::unique_ptr<StylePrivate> &&d)
    : QObject()
    , d(std::move(d))
{
}

Style::~Style() = default;

SelectorList Style::selectors() const
{
    return d->selectors;
}

void Style::setSelectors(const SelectorList &selectors)
{
    d->selectors = selectors;
}

QSizeF Style::contentSize() const
{
    const auto f = d->foreground.value_or(AreaDefinition{});
    const auto b = d->background.value_or(AreaDefinition{});

    return QSizeF{std::max(f.size.width(), b.size.width()), std::max(f.size.height(), b.size.height())};
}

QMarginsF Style::borderSizes() const
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

QRectF Style::boundingRect() const
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

std::optional<AreaDefinition> Style::foreground() const
{
    return d->foreground;
}

void Style::setForeground(const std::optional<AreaDefinition> &newForeground)
{
    d->foreground = newForeground;
}

std::optional<AreaDefinition> Style::background() const
{
    return d->background;
}

void Style::setBackground(const std::optional<AreaDefinition> &newBackground)
{
    d->background = newBackground;
}

std::optional<BorderDefinition> Style::border() const
{
    return d->border;
}

void Style::setBorder(const std::optional<BorderDefinition> &newBorder)
{
    d->border = newBorder;
}

std::optional<ShadowDefinition> Style::shadow() const
{
    return d->shadow;
}

void Style::setShadow(const std::optional<ShadowDefinition> &newShadow)
{
    d->shadow = newShadow;
}

std::optional<SizeDefinition> Style::margin() const
{
    return d->margin;
}

void Style::setMargin(const std::optional<SizeDefinition> &newMargin)
{
    d->margin = newMargin;
}

std::optional<SizeDefinition> Style::padding() const
{
    return d->padding;
}

void Style::setPadding(const std::optional<SizeDefinition> &newPadding)
{
    d->padding = newPadding;
}

std::optional<BorderDefinition> Style::outset() const
{
    return d->outset;
}

void Style::setOutset(const std::optional<BorderDefinition> &newOutset)
{
    d->outset = newOutset;
}

std::optional<CornersDefinition> Style::corners() const
{
    return d->corners;
}

void Style::setCorners(const std::optional<CornersDefinition> &newCorners)
{
    d->corners = newCorners;
}

Style::Ptr Style::create()
{
    return std::make_shared<Style>(std::make_unique<StylePrivate>());
}

QDebug operator<<(QDebug debug, std::shared_ptr<Union::Style> style)
{
    QDebugStateSaver saver(debug);
    debug << "Style(" << style->selectors() << ")";
    return debug;
}
