/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "StyleElement.h"

#include <QHash>

using namespace Union;

class UNION_NO_EXPORT StyleElement::Private
{
public:
    QString type;
    QString id;
    QString state;
    QStringList hints;
    QHash<QString, QVariant> attributes;

    std::optional<AreaDefinition> foreground;
    std::optional<AreaDefinition> background;
    std::optional<BorderDefinition> border;
    std::optional<ShadowDefinition> shadow;
    std::optional<SizeDefinition> margin;
    std::optional<SizeDefinition> padding;
    std::optional<BorderDefinition> outset;
    std::optional<CornersDefinition> corners;
};

StyleElement::StyleElement(const QString &type, QObject *parent)
    : QObject(parent)
    , d(std::make_unique<Private>())
{
    d->type = type;
}

StyleElement::~StyleElement() = default;

QString StyleElement::type() const
{
    return d->type;
}

QString StyleElement::id() const
{
    return d->id;
}

void StyleElement::setId(const QString &newId)
{
    d->id = newId;
}

QString StyleElement::state() const
{
    return d->state;
}

void StyleElement::setState(const QString &newState)
{
    d->state = newState;
}

QStringList StyleElement::hints() const
{
    return d->hints;
}

void StyleElement::setHints(const QStringList &newHints)
{
    d->hints = newHints;
}

QHash<QString, QVariant> StyleElement::attributes() const
{
    return d->attributes;
}

QVariant Union::StyleElement::attribute(const QString &name) const
{
    return d->attributes.value(name);
}

void StyleElement::addAttribute(const QString &name, const QVariant &value)
{
    d->attributes.insert(name, value);
}

void StyleElement::removeAttribute(const QString &name)
{
    d->attributes.remove(name);
}

QSizeF StyleElement::contentSize() const
{
    auto f = foreground().value_or(AreaDefinition{});
    auto b = background().value_or(AreaDefinition{});

    return QSizeF{std::max(f.size().width(), b.size().width()), std::max(f.size().height(), b.size().height())};
}

QMarginsF StyleElement::borderSizes() const
{
    auto b = border().value_or(BorderDefinition{});

    QMarginsF result;
    result.setLeft(b.left.value_or(LineDefinition{}).size.value());
    result.setRight(b.right.value_or(LineDefinition{}).size.value());
    result.setTop(b.top.value_or(LineDefinition{}).size.value());
    result.setBottom(b.bottom.value_or(LineDefinition{}).size.value());

    return result;
}

QRectF StyleElement::boundingRect() const
{
    QRectF result;

    auto c = contentSize();
    auto b = borderSizes();

    result.setWidth(b.left() + c.width() + b.right());
    result.setHeight(b.top() + c.height() + b.bottom());

    return result;
}

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
