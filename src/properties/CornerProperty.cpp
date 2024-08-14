// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

#include "CornerProperty.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

class Union::Properties::CornerPropertyPrivate
{
public:
    std::optional<qreal> radius;
    std::optional<qreal> width;
    std::optional<qreal> height;
    std::optional<QColor> color;
    std::optional<ImageProperty> image;
};

CornerProperty::CornerProperty()
    : d(std::make_unique<CornerPropertyPrivate>())
{
}

CornerProperty::CornerProperty(const CornerProperty &other)
    : d(std::make_unique<CornerPropertyPrivate>())
{
    d->radius = other.d->radius;
    d->width = other.d->width;
    d->height = other.d->height;
    d->color = other.d->color;
    d->image = other.d->image;
}

CornerProperty::CornerProperty(CornerProperty &&other)
    : d(std::move(other.d))
{
}

CornerProperty::~CornerProperty() = default;

CornerProperty &CornerProperty::operator=(const CornerProperty &other)
{
    if (this != &other) {
        d->radius = other.d->radius;
        d->width = other.d->width;
        d->height = other.d->height;
        d->color = other.d->color;
        d->image = other.d->image;
    }
    return *this;
}

CornerProperty &CornerProperty::operator=(CornerProperty &&other)
{
    std::swap(d, other.d);
    return *this;
}

std::optional<qreal> CornerProperty::radius() const
{
    return d->radius;
}

void CornerProperty::setRadius(const std::optional<qreal> &newValue)
{
    if (newValue == d->radius) {
        return;
    }

    d->radius = newValue;
}
std::optional<qreal> CornerProperty::width() const
{
    return d->width;
}

void CornerProperty::setWidth(const std::optional<qreal> &newValue)
{
    if (newValue == d->width) {
        return;
    }

    d->width = newValue;
}
std::optional<qreal> CornerProperty::height() const
{
    return d->height;
}

void CornerProperty::setHeight(const std::optional<qreal> &newValue)
{
    if (newValue == d->height) {
        return;
    }

    d->height = newValue;
}
std::optional<QColor> CornerProperty::color() const
{
    return d->color;
}

void CornerProperty::setColor(const std::optional<QColor> &newValue)
{
    if (newValue == d->color) {
        return;
    }

    d->color = newValue;
}
std::optional<ImageProperty> CornerProperty::image() const
{
    return d->image;
}

void CornerProperty::setImage(const std::optional<ImageProperty> &newValue)
{
    if (newValue == d->image) {
        return;
    }

    d->image = newValue;
}

bool CornerProperty::hasAnyValue() const
{
    if (d->radius.has_value()) {
        return true;
    }
    if (d->width.has_value()) {
        return true;
    }
    if (d->height.has_value()) {
        return true;
    }
    if (d->color.has_value()) {
        return true;
    }
    if (d->image.has_value() && d->image->hasAnyValue()) {
        return true;
    }
    return false;
}

void CornerProperty::resolveProperties(const CornerProperty &source, CornerProperty &destination)
{
    if (!destination.d->radius.has_value()) {
        destination.d->radius = source.d->radius;
    }
    if (!destination.d->width.has_value()) {
        destination.d->width = source.d->width;
    }
    if (!destination.d->height.has_value()) {
        destination.d->height = source.d->height;
    }
    if (!destination.d->color.has_value()) {
        destination.d->color = source.d->color;
    }
    if (source.d->image.has_value()) {
        ImageProperty value;
        if (destination.d->image.has_value()) {
            value = destination.d->image.value();
        }
        ImageProperty::resolveProperties(source.d->image.value(), value);
        if (value.hasAnyValue()) {
            destination.d->image = value;
        }
    }
}

bool Union::Properties::operator==(const CornerProperty &left, const CornerProperty &right)
{
    if (left.radius() != right.radius()) {
        return false;
    }
    if (left.width() != right.width()) {
        return false;
    }
    if (left.height() != right.height()) {
        return false;
    }
    if (left.color() != right.color()) {
        return false;
    }
    if (left.image() != right.image()) {
        return false;
    }
    return true;
}

QDebug operator<<(QDebug debug, const Union::Properties::CornerProperty &type)
{
    QDebugStateSaver saver(debug);
    debug << "CornerProperty(";
    debug << "  radius:" << type.radius();
    debug << "  width:" << type.width();
    debug << "  height:" << type.height();
    debug << "  color:" << type.color();
    debug << "  image:" << type.image();
    debug << ")";
    return debug;
}