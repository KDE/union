// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#include "Color.h"

#include <QSharedData>

#include "PluginRegistry.h"

#include "union_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

struct RgbaData : ColorData {
    RgbaData(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
        : ColorData(Type::RGBA)
        , r(_r)
        , g(_g)
        , b(_b)
        , a(_a)
    {
    }

    RgbaData(const RgbaData &other)
        : ColorData(other)
    {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
    }

    inline ColorData *toRgba() const override
    {
        return new RgbaData(r, g, b, a);
    }

    inline QString toString() const override
    {
        return u"RGBA, %1, %2, %3, %4"_s.arg(r).arg(g).arg(b).arg(a);
    }

    inline bool equals(const ColorData *other) const override
    {
        auto otherRgba = static_cast<const RgbaData *>(other);
        return r == otherRgba->r && g == otherRgba->g && b == otherRgba->b && a == otherRgba->a;
    }

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 0;
};

struct CustomData : ColorData {
    CustomData(const QString &_source, const QStringList &_arguments)
        : ColorData(Type::Custom)
        , source(_source)
        , arguments(_arguments)
    {
        if (!s_colorProviderRegistry) {
            s_colorProviderRegistry = std::make_shared<PluginRegistry<ColorProvider>>(QJsonObject{{u"union-plugintype"_s, u"colorprovider"_s}});
        }
    }

    CustomData(const CustomData &other)
        : ColorData(other)
    {
        source = other.source;
        arguments = other.arguments;
    }

    inline ColorData *toRgba() const override
    {
        auto provider = s_knownProviders.value(source);
        if (!provider) {
            provider = s_colorProviderRegistry->pluginObject(u"union-colorprovider-"_s + source);
            if (!provider) {
                qCWarning(UNION_GENERAL) << "No provider" << source << "found";
                return nullptr;
            }
        }

        auto colorValue = provider->color(arguments);
        if (colorValue) {
            auto rgba = colorValue.value();
            return new RgbaData(rgba.r, rgba.g, rgba.b, rgba.a);
        }

        return nullptr;
    }

    inline QString toString() const override
    {
        return u"Custom, source: %1, arguments: %2"_s.arg(source, arguments.join(u", "));
    }

    inline bool equals(const ColorData *other) const override
    {
        auto otherCustom = static_cast<const CustomData *>(other);
        return source == otherCustom->source && arguments == otherCustom->arguments;
    }

    QString source;
    QStringList arguments;

    inline static std::shared_ptr<PluginRegistry<ColorProvider>> s_colorProviderRegistry;
    inline static QHash<QString, ColorProvider *> s_knownProviders;
};

struct MixData : ColorData {
    MixData(const Color &_first, const Color &_second, qreal _amount)
        : ColorData(Type::Mix)
        , first(_first)
        , second(_second)
        , amount(_amount)
    {
    }

    MixData(const MixData &other)
        : ColorData(other)
    {
        first = other.first;
        second = other.second;
        amount = other.amount;
    }

    inline ColorData *toRgba() const override
    {
        auto firstRgba = first.toRgba();
        auto secondRgba = second.toRgba();

        auto invAmount = 1.0 - amount;

        return new RgbaData{
            uint8_t(firstRgba.red() * invAmount + secondRgba.red() * amount),
            uint8_t(firstRgba.green() * invAmount + secondRgba.green() * amount),
            uint8_t(firstRgba.blue() * invAmount + secondRgba.blue() * amount),
            uint8_t(firstRgba.alpha() * invAmount + secondRgba.alpha() * amount),
        };
    }

    inline QString toString() const override
    {
        return u"Mix, first: %1, second: %2, amount: %3"_s.arg(first.toString(), second.toString()).arg(amount);
    }

    inline bool equals(const ColorData *other) const override
    {
        auto otherMix = static_cast<const MixData *>(other);
        return first == otherMix->first && second == otherMix->second && qFuzzyCompare(amount, otherMix->amount);
    }

    Color first;
    Color second;
    qreal amount;
};

Color::Color()
{
}

Color::Color(ColorData *_data)
    : data(_data)
{
}

Color::Color(const Color &other)
{
    *this = other;
}

Color &Color::operator=(const Color &other)
{
    data = other.data;
    return *this;
}

bool Color::operator==(const Color &other) const
{
    if (data == other.data) {
        return true;
    }

    if (data->type != other.data->type) {
        return false;
    }

    return data->equals(other.data.get());
}

bool Color::isValid() const
{
    return bool(data);
}

ColorData::Type Color::type() const
{
    return data->type;
}

uint8_t Color::red() const
{
    if (data->type == ColorData::Type::RGBA) {
        return static_cast<const RgbaData *>(data.get())->r;
    }

    return 0;
}

uint8_t Color::green() const
{
    if (data->type == ColorData::Type::RGBA) {
        return static_cast<const RgbaData *>(data.get())->g;
    }

    return 0;
}

uint8_t Color::blue() const
{
    if (data->type == ColorData::Type::RGBA) {
        return static_cast<const RgbaData *>(data.get())->b;
    }

    return 0;
}

uint8_t Color::alpha() const
{
    if (data->type == ColorData::Type::RGBA) {
        return static_cast<const RgbaData *>(data.get())->a;
    }

    return 0;
}

Color Color::toRgba() const
{
    if (!data) {
        return Color{};
    }

    if (data->type == ColorData::Type::RGBA) {
        return *this;
    }

    return Color{data->toRgba()};
}

QColor Color::toQColor() const
{
    const Color rgba = toRgba();
    if (!rgba.isValid()) {
        return Qt::transparent;
    }

    auto rgbaData = static_cast<const RgbaData *>(rgba.data.get());
    return QColor(rgbaData->r, rgbaData->g, rgbaData->b, rgbaData->a);
}

QString Color::toString() const
{
    return u"Color("_s + data->toString() + u")"_s;
}

Color Color::rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return Color(new RgbaData(r, g, b, a));
}

Color Color::custom(const QString &source, const QStringList &arguments)
{
    return Color(new CustomData(source, arguments));
}

Color Color::mix(const Color &first, const Color &second, qreal amount)
{
    return Color(new MixData(first, second, amount));
}

void ColorProvider::registerProvider(const QString &name, ColorProvider *provider)
{
    CustomData::s_knownProviders.insert(name, provider);
}

QDebug &operator<<(QDebug &stream, const Union::Color &color)
{
    stream << color.toString();
    return stream;
}
