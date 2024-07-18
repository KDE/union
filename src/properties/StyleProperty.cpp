// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `generate_properties.py`.

#include "StyleProperty.h"

using namespace Union::Properties;
using namespace Qt::StringLiterals;

class Union::Properties::StylePropertyPrivate
{
public:
    std::optional<PaletteProperty> palette;
    std::optional<SizesProperty> sizes;
    std::optional<TextProperty> text;
    std::optional<IndicatorProperty> indicator;
    std::optional<BackgroundProperty> background;
};

StyleProperty::StyleProperty()
    : d(std::make_unique<StylePropertyPrivate>())
{
}

StyleProperty::StyleProperty(const StyleProperty &other)
    : d(std::make_unique<StylePropertyPrivate>())
{
    d->palette = other.d->palette;
    d->sizes = other.d->sizes;
    d->text = other.d->text;
    d->indicator = other.d->indicator;
    d->background = other.d->background;
}

StyleProperty::StyleProperty(StyleProperty &&other)
    : d(std::move(other.d))
{
}

StyleProperty::~StyleProperty() = default;

StyleProperty &StyleProperty::operator=(const StyleProperty &other)
{
    if (this != &other) {
        d->palette = other.d->palette;
        d->sizes = other.d->sizes;
        d->text = other.d->text;
        d->indicator = other.d->indicator;
        d->background = other.d->background;
    }
    return *this;
}

StyleProperty &StyleProperty::operator=(StyleProperty &&other)
{
    std::swap(d, other.d);
    return *this;
}

std::optional<PaletteProperty> StyleProperty::palette() const
{
    return d->palette;
}

void StyleProperty::setPalette(const std::optional<PaletteProperty> &newValue)
{
    if (newValue == d->palette) {
        return;
    }

    d->palette = newValue;
}
std::optional<SizesProperty> StyleProperty::sizes() const
{
    return d->sizes;
}

void StyleProperty::setSizes(const std::optional<SizesProperty> &newValue)
{
    if (newValue == d->sizes) {
        return;
    }

    d->sizes = newValue;
}
std::optional<TextProperty> StyleProperty::text() const
{
    return d->text;
}

void StyleProperty::setText(const std::optional<TextProperty> &newValue)
{
    if (newValue == d->text) {
        return;
    }

    d->text = newValue;
}
std::optional<IndicatorProperty> StyleProperty::indicator() const
{
    return d->indicator;
}

void StyleProperty::setIndicator(const std::optional<IndicatorProperty> &newValue)
{
    if (newValue == d->indicator) {
        return;
    }

    d->indicator = newValue;
}
std::optional<BackgroundProperty> StyleProperty::background() const
{
    return d->background;
}

void StyleProperty::setBackground(const std::optional<BackgroundProperty> &newValue)
{
    if (newValue == d->background) {
        return;
    }

    d->background = newValue;
}

bool StyleProperty::hasAnyValue() const
{
    if (d->palette.has_value() && d->palette->hasAnyValue()) {
        return true;
    }
    if (d->sizes.has_value() && d->sizes->hasAnyValue()) {
        return true;
    }
    if (d->text.has_value() && d->text->hasAnyValue()) {
        return true;
    }
    if (d->indicator.has_value() && d->indicator->hasAnyValue()) {
        return true;
    }
    if (d->background.has_value() && d->background->hasAnyValue()) {
        return true;
    }
    return false;
}

void StyleProperty::resolveProperties(const StyleProperty &source, StyleProperty &destination)
{
    if (source.d->palette.has_value()) {
        PaletteProperty value;
        if (destination.d->palette.has_value()) {
            value = destination.d->palette.value();
        }
        PaletteProperty::resolveProperties(source.d->palette.value(), value);
        if (value.hasAnyValue()) {
            destination.d->palette = value;
        }
    }
    if (source.d->sizes.has_value()) {
        SizesProperty value;
        if (destination.d->sizes.has_value()) {
            value = destination.d->sizes.value();
        }
        SizesProperty::resolveProperties(source.d->sizes.value(), value);
        if (value.hasAnyValue()) {
            destination.d->sizes = value;
        }
    }
    if (source.d->text.has_value()) {
        TextProperty value;
        if (destination.d->text.has_value()) {
            value = destination.d->text.value();
        }
        TextProperty::resolveProperties(source.d->text.value(), value);
        if (value.hasAnyValue()) {
            destination.d->text = value;
        }
    }
    if (source.d->indicator.has_value()) {
        IndicatorProperty value;
        if (destination.d->indicator.has_value()) {
            value = destination.d->indicator.value();
        }
        IndicatorProperty::resolveProperties(source.d->indicator.value(), value);
        if (value.hasAnyValue()) {
            destination.d->indicator = value;
        }
    }
    if (source.d->background.has_value()) {
        BackgroundProperty value;
        if (destination.d->background.has_value()) {
            value = destination.d->background.value();
        }
        BackgroundProperty::resolveProperties(source.d->background.value(), value);
        if (value.hasAnyValue()) {
            destination.d->background = value;
        }
    }
}

bool Union::Properties::operator==(const StyleProperty &left, const StyleProperty &right)
{
    if (left.palette() != right.palette()) {
        return false;
    }
    if (left.sizes() != right.sizes()) {
        return false;
    }
    if (left.text() != right.text()) {
        return false;
    }
    if (left.indicator() != right.indicator()) {
        return false;
    }
    if (left.background() != right.background()) {
        return false;
    }
    return true;
}

QDebug operator<<(QDebug debug, const Union::Properties::StyleProperty &type)
{
    QDebugStateSaver saver(debug);
    debug << "StyleProperty(";
    debug << "  palette:" << type.palette();
    debug << "  sizes:" << type.sizes();
    debug << "  text:" << type.text();
    debug << "  indicator:" << type.indicator();
    debug << "  background:" << type.background();
    debug << ")";
    return debug;
}