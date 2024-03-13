/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "StyleElement.h"

using namespace Union;

class UNION_NO_EXPORT StyleElement::Private
{
public:

    std::optional<AreaDefinition> foreground;
    std::optional<AreaDefinition> background;
    std::optional<BorderDefinition> border;
    std::optional<ShadowDefinition> shadow;
    std::optional<SizeDefinition> margin;
    std::optional<SizeDefinition> padding;
    std::optional<BorderDefinition> outset;
    std::optional<CornersDefinition> corners;
};

StyleElement::StyleElement(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<Private>())
{
}

StyleElement::~StyleElement() = default;
std::optional<AreaDefinition> StyleElement::foreground() const
{
    if (!d->foreground && d->parent) {
        return d->parent->foreground();
    }

    return d->foreground;
}

void StyleElement::setForeground(const std::optional<AreaDefinition> &newForeground)
{
    d->foreground = newForeground;
}

std::optional<AreaDefinition> StyleElement::background() const
{
    if (!d->background && d->parent) {
        return d->parent->background();
    }

    return d->background;
}

void StyleElement::setBackground(const std::optional<AreaDefinition> &newBackground)
{
    d->background = newBackground;
}

std::optional<BorderDefinition> StyleElement::border() const
{
    if (!d->border && d->parent) {
        return d->parent->border();
    }

    return d->border;
}

void StyleElement::setBorder(const std::optional<BorderDefinition> &newBorder)
{
    d->border = newBorder;
}

std::optional<ShadowDefinition> StyleElement::shadow() const
{
    if (!d->shadow && d->parent) {
        return d->parent->shadow();
    }

    return d->shadow;
}

void StyleElement::setShadow(const std::optional<ShadowDefinition> &newShadow)
{
    d->shadow = newShadow;
}

std::optional<SizeDefinition> StyleElement::margin() const
{
    if (!d->margin && d->parent) {
        return d->parent->margin();
    }

    return d->margin;
}

void StyleElement::setMargin(const std::optional<SizeDefinition> &newMargin)
{
    d->margin = newMargin;
}

std::optional<SizeDefinition> StyleElement::padding() const
{
    if (!d->padding && d->parent) {
        return d->parent->padding();
    }

    return d->padding;
}

void StyleElement::setPadding(const std::optional<SizeDefinition> &newPadding)
{
    d->padding = newPadding;
}

std::optional<BorderDefinition> StyleElement::outset() const
{
    if (!d->outset && d->parent) {
        return d->parent->outset();
    }

    return d->outset;
}

void StyleElement::setOutset(const std::optional<BorderDefinition> &newOutset)
{
    d->outset = newOutset;
}

std::optional<CornersDefinition> StyleElement::corners() const
{
    if (!d->corners && d->parent) {
        return d->parent->corners();
    }

    return d->corners;
}

void StyleElement::setCorners(const std::optional<CornersDefinition> &newCorners)
{
    d->corners = newCorners;
}
