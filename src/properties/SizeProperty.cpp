// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

#include "SizeProperty.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

class Union::Properties::SizePropertyPrivate
{
public:
    std::optional<qreal> left;
    std::optional<qreal> right;
    std::optional<qreal> top;
    std::optional<qreal> bottom;
};

SizeProperty::SizeProperty()
    : d(std::make_unique<SizePropertyPrivate>())
{
}

SizeProperty::SizeProperty(const SizeProperty &other)
    : d(std::make_unique<SizePropertyPrivate>())
{
    d->left = other.d->left;
    d->right = other.d->right;
    d->top = other.d->top;
    d->bottom = other.d->bottom;
}

SizeProperty::SizeProperty(SizeProperty &&other)
    : d(std::move(other.d))
{
}

SizeProperty::~SizeProperty() = default;

SizeProperty &SizeProperty::operator=(const SizeProperty &other)
{
    if (this != &other) {
        d->left = other.d->left;
        d->right = other.d->right;
        d->top = other.d->top;
        d->bottom = other.d->bottom;
    }
    return *this;
}

SizeProperty &SizeProperty::operator=(SizeProperty &&other)
{
    std::swap(d, other.d);
    return *this;
}

std::optional<qreal> SizeProperty::left() const
{
    return d->left;
}

void SizeProperty::setLeft(const std::optional<qreal> &newValue)
{
    if (newValue == d->left) {
        return;
    }

    d->left = newValue;
}
std::optional<qreal> SizeProperty::right() const
{
    return d->right;
}

void SizeProperty::setRight(const std::optional<qreal> &newValue)
{
    if (newValue == d->right) {
        return;
    }

    d->right = newValue;
}
std::optional<qreal> SizeProperty::top() const
{
    return d->top;
}

void SizeProperty::setTop(const std::optional<qreal> &newValue)
{
    if (newValue == d->top) {
        return;
    }

    d->top = newValue;
}
std::optional<qreal> SizeProperty::bottom() const
{
    return d->bottom;
}

void SizeProperty::setBottom(const std::optional<qreal> &newValue)
{
    if (newValue == d->bottom) {
        return;
    }

    d->bottom = newValue;
}

bool SizeProperty::hasAnyValue() const
{
    if (d->left.has_value()) {
        return true;
    }
    if (d->right.has_value()) {
        return true;
    }
    if (d->top.has_value()) {
        return true;
    }
    if (d->bottom.has_value()) {
        return true;
    }
    return false;
}

void SizeProperty::resolveProperties(const SizeProperty &source, SizeProperty &destination)
{
    if (!destination.d->left.has_value()) {
        destination.d->left = source.d->left;
    }
    if (!destination.d->right.has_value()) {
        destination.d->right = source.d->right;
    }
    if (!destination.d->top.has_value()) {
        destination.d->top = source.d->top;
    }
    if (!destination.d->bottom.has_value()) {
        destination.d->bottom = source.d->bottom;
    }
}

bool Union::Properties::operator==(const SizeProperty &left, const SizeProperty &right)
{
    if (left.left() != right.left()) {
        return false;
    }
    if (left.right() != right.right()) {
        return false;
    }
    if (left.top() != right.top()) {
        return false;
    }
    if (left.bottom() != right.bottom()) {
        return false;
    }
    return true;
}

QDebug operator<<(QDebug debug, const Union::Properties::SizeProperty &type)
{
    QDebugStateSaver saver(debug);
    debug << "SizeProperty(";
    debug << "  left:" << type.left();
    debug << "  right:" << type.right();
    debug << "  top:" << type.top();
    debug << "  bottom:" << type.bottom();
    debug << ")";
    return debug;
}