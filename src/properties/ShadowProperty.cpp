// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `generate_properties.py`.

#include "ShadowProperty.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

class Union::Properties::ShadowPropertyPrivate
{
public:
    std::optional<SizeProperty> offsets;
    std::optional<BorderProperty> left;
    std::optional<BorderProperty> right;
    std::optional<BorderProperty> top;
    std::optional<BorderProperty> bottom;
    std::optional<CornerProperty> topLeft;
    std::optional<CornerProperty> topRight;
    std::optional<CornerProperty> bottomLeft;
    std::optional<CornerProperty> bottomRight;
};

ShadowProperty::ShadowProperty()
    : d(std::make_unique<ShadowPropertyPrivate>())
{
}

ShadowProperty::ShadowProperty(const ShadowProperty &other)
    : d(std::make_unique<ShadowPropertyPrivate>())
{
    d->offsets = other.d->offsets;
    d->left = other.d->left;
    d->right = other.d->right;
    d->top = other.d->top;
    d->bottom = other.d->bottom;
    d->topLeft = other.d->topLeft;
    d->topRight = other.d->topRight;
    d->bottomLeft = other.d->bottomLeft;
    d->bottomRight = other.d->bottomRight;
}

ShadowProperty::ShadowProperty(ShadowProperty &&other)
    : d(std::move(other.d))
{
}

ShadowProperty::~ShadowProperty() = default;

ShadowProperty &ShadowProperty::operator=(const ShadowProperty &other)
{
    if (this != &other) {
        d->offsets = other.d->offsets;
        d->left = other.d->left;
        d->right = other.d->right;
        d->top = other.d->top;
        d->bottom = other.d->bottom;
        d->topLeft = other.d->topLeft;
        d->topRight = other.d->topRight;
        d->bottomLeft = other.d->bottomLeft;
        d->bottomRight = other.d->bottomRight;
    }
    return *this;
}

ShadowProperty &ShadowProperty::operator=(ShadowProperty &&other)
{
    std::swap(d, other.d);
    return *this;
}

std::optional<SizeProperty> ShadowProperty::offsets() const
{
    return d->offsets;
}

void ShadowProperty::setOffsets(const std::optional<SizeProperty> &newValue)
{
    if (newValue == d->offsets) {
        return;
    }

    d->offsets = newValue;
}
std::optional<BorderProperty> ShadowProperty::left() const
{
    return d->left;
}

void ShadowProperty::setLeft(const std::optional<BorderProperty> &newValue)
{
    if (newValue == d->left) {
        return;
    }

    d->left = newValue;
}
std::optional<BorderProperty> ShadowProperty::right() const
{
    return d->right;
}

void ShadowProperty::setRight(const std::optional<BorderProperty> &newValue)
{
    if (newValue == d->right) {
        return;
    }

    d->right = newValue;
}
std::optional<BorderProperty> ShadowProperty::top() const
{
    return d->top;
}

void ShadowProperty::setTop(const std::optional<BorderProperty> &newValue)
{
    if (newValue == d->top) {
        return;
    }

    d->top = newValue;
}
std::optional<BorderProperty> ShadowProperty::bottom() const
{
    return d->bottom;
}

void ShadowProperty::setBottom(const std::optional<BorderProperty> &newValue)
{
    if (newValue == d->bottom) {
        return;
    }

    d->bottom = newValue;
}
std::optional<CornerProperty> ShadowProperty::topLeft() const
{
    return d->topLeft;
}

void ShadowProperty::setTopLeft(const std::optional<CornerProperty> &newValue)
{
    if (newValue == d->topLeft) {
        return;
    }

    d->topLeft = newValue;
}
std::optional<CornerProperty> ShadowProperty::topRight() const
{
    return d->topRight;
}

void ShadowProperty::setTopRight(const std::optional<CornerProperty> &newValue)
{
    if (newValue == d->topRight) {
        return;
    }

    d->topRight = newValue;
}
std::optional<CornerProperty> ShadowProperty::bottomLeft() const
{
    return d->bottomLeft;
}

void ShadowProperty::setBottomLeft(const std::optional<CornerProperty> &newValue)
{
    if (newValue == d->bottomLeft) {
        return;
    }

    d->bottomLeft = newValue;
}
std::optional<CornerProperty> ShadowProperty::bottomRight() const
{
    return d->bottomRight;
}

void ShadowProperty::setBottomRight(const std::optional<CornerProperty> &newValue)
{
    if (newValue == d->bottomRight) {
        return;
    }

    d->bottomRight = newValue;
}

bool ShadowProperty::hasAnyValue() const
{
    if (d->offsets.has_value() && d->offsets->hasAnyValue()) {
        return true;
    }
    if (d->left.has_value() && d->left->hasAnyValue()) {
        return true;
    }
    if (d->right.has_value() && d->right->hasAnyValue()) {
        return true;
    }
    if (d->top.has_value() && d->top->hasAnyValue()) {
        return true;
    }
    if (d->bottom.has_value() && d->bottom->hasAnyValue()) {
        return true;
    }
    if (d->topLeft.has_value() && d->topLeft->hasAnyValue()) {
        return true;
    }
    if (d->topRight.has_value() && d->topRight->hasAnyValue()) {
        return true;
    }
    if (d->bottomLeft.has_value() && d->bottomLeft->hasAnyValue()) {
        return true;
    }
    if (d->bottomRight.has_value() && d->bottomRight->hasAnyValue()) {
        return true;
    }
    return false;
}

void ShadowProperty::resolveProperties(const ShadowProperty &source, ShadowProperty &destination)
{
    if (source.d->offsets.has_value()) {
        SizeProperty value;
        if (destination.d->offsets.has_value()) {
            value = destination.d->offsets.value();
        }
        SizeProperty::resolveProperties(source.d->offsets.value(), value);
        if (value.hasAnyValue()) {
            destination.d->offsets = value;
        }
    }
    if (source.d->left.has_value()) {
        BorderProperty value;
        if (destination.d->left.has_value()) {
            value = destination.d->left.value();
        }
        BorderProperty::resolveProperties(source.d->left.value(), value);
        if (value.hasAnyValue()) {
            destination.d->left = value;
        }
    }
    if (source.d->right.has_value()) {
        BorderProperty value;
        if (destination.d->right.has_value()) {
            value = destination.d->right.value();
        }
        BorderProperty::resolveProperties(source.d->right.value(), value);
        if (value.hasAnyValue()) {
            destination.d->right = value;
        }
    }
    if (source.d->top.has_value()) {
        BorderProperty value;
        if (destination.d->top.has_value()) {
            value = destination.d->top.value();
        }
        BorderProperty::resolveProperties(source.d->top.value(), value);
        if (value.hasAnyValue()) {
            destination.d->top = value;
        }
    }
    if (source.d->bottom.has_value()) {
        BorderProperty value;
        if (destination.d->bottom.has_value()) {
            value = destination.d->bottom.value();
        }
        BorderProperty::resolveProperties(source.d->bottom.value(), value);
        if (value.hasAnyValue()) {
            destination.d->bottom = value;
        }
    }
    if (source.d->topLeft.has_value()) {
        CornerProperty value;
        if (destination.d->topLeft.has_value()) {
            value = destination.d->topLeft.value();
        }
        CornerProperty::resolveProperties(source.d->topLeft.value(), value);
        if (value.hasAnyValue()) {
            destination.d->topLeft = value;
        }
    }
    if (source.d->topRight.has_value()) {
        CornerProperty value;
        if (destination.d->topRight.has_value()) {
            value = destination.d->topRight.value();
        }
        CornerProperty::resolveProperties(source.d->topRight.value(), value);
        if (value.hasAnyValue()) {
            destination.d->topRight = value;
        }
    }
    if (source.d->bottomLeft.has_value()) {
        CornerProperty value;
        if (destination.d->bottomLeft.has_value()) {
            value = destination.d->bottomLeft.value();
        }
        CornerProperty::resolveProperties(source.d->bottomLeft.value(), value);
        if (value.hasAnyValue()) {
            destination.d->bottomLeft = value;
        }
    }
    if (source.d->bottomRight.has_value()) {
        CornerProperty value;
        if (destination.d->bottomRight.has_value()) {
            value = destination.d->bottomRight.value();
        }
        CornerProperty::resolveProperties(source.d->bottomRight.value(), value);
        if (value.hasAnyValue()) {
            destination.d->bottomRight = value;
        }
    }
}

bool Union::Properties::operator==(const ShadowProperty &left, const ShadowProperty &right)
{
    if (left.offsets() != right.offsets()) {
        return false;
    }
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
    if (left.topLeft() != right.topLeft()) {
        return false;
    }
    if (left.topRight() != right.topRight()) {
        return false;
    }
    if (left.bottomLeft() != right.bottomLeft()) {
        return false;
    }
    if (left.bottomRight() != right.bottomRight()) {
        return false;
    }
    return true;
}

QDebug operator<<(QDebug debug, const Union::Properties::ShadowProperty &type)
{
    QDebugStateSaver saver(debug);
    debug << "ShadowProperty(";
    debug << "  offsets:" << type.offsets();
    debug << "  left:" << type.left();
    debug << "  right:" << type.right();
    debug << "  top:" << type.top();
    debug << "  bottom:" << type.bottom();
    debug << "  topLeft:" << type.topLeft();
    debug << "  topRight:" << type.topRight();
    debug << "  bottomLeft:" << type.bottomLeft();
    debug << "  bottomRight:" << type.bottomRight();
    debug << ")";
    return debug;
}