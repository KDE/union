// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

#include "LayoutProperty.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

class Union::Properties::LayoutPropertyPrivate
{
public:
    std::optional<AlignmentProperty> alignment;
    std::optional<qreal> width;
    std::optional<qreal> height;
    std::optional<qreal> spacing;
    std::optional<SizeProperty> padding;
    std::optional<SizeProperty> inset;
    std::optional<SizeProperty> margins;
};

LayoutProperty::LayoutProperty()
    : d(std::make_unique<LayoutPropertyPrivate>())
{
}

LayoutProperty::LayoutProperty(const LayoutProperty &other)
    : d(std::make_unique<LayoutPropertyPrivate>())
{
    d->alignment = other.d->alignment;
    d->width = other.d->width;
    d->height = other.d->height;
    d->spacing = other.d->spacing;
    d->padding = other.d->padding;
    d->inset = other.d->inset;
    d->margins = other.d->margins;
}

LayoutProperty::LayoutProperty(LayoutProperty &&other)
    : d(std::move(other.d))
{
}

LayoutProperty::~LayoutProperty() = default;

LayoutProperty &LayoutProperty::operator=(const LayoutProperty &other)
{
    if (this != &other) {
        d->alignment = other.d->alignment;
        d->width = other.d->width;
        d->height = other.d->height;
        d->spacing = other.d->spacing;
        d->padding = other.d->padding;
        d->inset = other.d->inset;
        d->margins = other.d->margins;
    }
    return *this;
}

LayoutProperty &LayoutProperty::operator=(LayoutProperty &&other)
{
    std::swap(d, other.d);
    return *this;
}

std::optional<AlignmentProperty> LayoutProperty::alignment() const
{
    return d->alignment;
}

void LayoutProperty::setAlignment(const std::optional<AlignmentProperty> &newValue)
{
    if (newValue == d->alignment) {
        return;
    }

    d->alignment = newValue;
}
std::optional<qreal> LayoutProperty::width() const
{
    return d->width;
}

void LayoutProperty::setWidth(const std::optional<qreal> &newValue)
{
    if (newValue == d->width) {
        return;
    }

    d->width = newValue;
}
std::optional<qreal> LayoutProperty::height() const
{
    return d->height;
}

void LayoutProperty::setHeight(const std::optional<qreal> &newValue)
{
    if (newValue == d->height) {
        return;
    }

    d->height = newValue;
}
std::optional<qreal> LayoutProperty::spacing() const
{
    return d->spacing;
}

void LayoutProperty::setSpacing(const std::optional<qreal> &newValue)
{
    if (newValue == d->spacing) {
        return;
    }

    d->spacing = newValue;
}
std::optional<SizeProperty> LayoutProperty::padding() const
{
    return d->padding;
}

void LayoutProperty::setPadding(const std::optional<SizeProperty> &newValue)
{
    if (newValue == d->padding) {
        return;
    }

    d->padding = newValue;
}
std::optional<SizeProperty> LayoutProperty::inset() const
{
    return d->inset;
}

void LayoutProperty::setInset(const std::optional<SizeProperty> &newValue)
{
    if (newValue == d->inset) {
        return;
    }

    d->inset = newValue;
}
std::optional<SizeProperty> LayoutProperty::margins() const
{
    return d->margins;
}

void LayoutProperty::setMargins(const std::optional<SizeProperty> &newValue)
{
    if (newValue == d->margins) {
        return;
    }

    d->margins = newValue;
}

bool LayoutProperty::hasAnyValue() const
{
    if (d->alignment.has_value() && d->alignment->hasAnyValue()) {
        return true;
    }
    if (d->width.has_value()) {
        return true;
    }
    if (d->height.has_value()) {
        return true;
    }
    if (d->spacing.has_value()) {
        return true;
    }
    if (d->padding.has_value() && d->padding->hasAnyValue()) {
        return true;
    }
    if (d->inset.has_value() && d->inset->hasAnyValue()) {
        return true;
    }
    if (d->margins.has_value() && d->margins->hasAnyValue()) {
        return true;
    }
    return false;
}

void LayoutProperty::resolveProperties(const LayoutProperty &source, LayoutProperty &destination)
{
    if (source.d->alignment.has_value()) {
        AlignmentProperty value;
        if (destination.d->alignment.has_value()) {
            value = destination.d->alignment.value();
        }
        AlignmentProperty::resolveProperties(source.d->alignment.value(), value);
        if (value.hasAnyValue()) {
            destination.d->alignment = value;
        }
    }
    if (!destination.d->width.has_value()) {
        destination.d->width = source.d->width;
    }
    if (!destination.d->height.has_value()) {
        destination.d->height = source.d->height;
    }
    if (!destination.d->spacing.has_value()) {
        destination.d->spacing = source.d->spacing;
    }
    if (source.d->padding.has_value()) {
        SizeProperty value;
        if (destination.d->padding.has_value()) {
            value = destination.d->padding.value();
        }
        SizeProperty::resolveProperties(source.d->padding.value(), value);
        if (value.hasAnyValue()) {
            destination.d->padding = value;
        }
    }
    if (source.d->inset.has_value()) {
        SizeProperty value;
        if (destination.d->inset.has_value()) {
            value = destination.d->inset.value();
        }
        SizeProperty::resolveProperties(source.d->inset.value(), value);
        if (value.hasAnyValue()) {
            destination.d->inset = value;
        }
    }
    if (source.d->margins.has_value()) {
        SizeProperty value;
        if (destination.d->margins.has_value()) {
            value = destination.d->margins.value();
        }
        SizeProperty::resolveProperties(source.d->margins.value(), value);
        if (value.hasAnyValue()) {
            destination.d->margins = value;
        }
    }
}

bool Union::Properties::operator==(const LayoutProperty &left, const LayoutProperty &right)
{
    if (left.alignment() != right.alignment()) {
        return false;
    }
    if (left.width() != right.width()) {
        return false;
    }
    if (left.height() != right.height()) {
        return false;
    }
    if (left.spacing() != right.spacing()) {
        return false;
    }
    if (left.padding() != right.padding()) {
        return false;
    }
    if (left.inset() != right.inset()) {
        return false;
    }
    if (left.margins() != right.margins()) {
        return false;
    }
    return true;
}

QDebug operator<<(QDebug debug, const Union::Properties::LayoutProperty &type)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "LayoutProperty(" //
                    << "alignment: " << type.alignment() //
                    << ", width: " << type.width() //
                    << ", height: " << type.height() //
                    << ", spacing: " << type.spacing() //
                    << ", padding: " << type.padding() //
                    << ", inset: " << type.inset() //
                    << ", margins: " << type.margins() //
                    << ")";
    return debug;
}