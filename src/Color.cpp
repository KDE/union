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

struct ColorOperationData : ColorData {
    ColorOperationData(ColorData::Type type, const Color &_color, const Color &_other)
        : ColorData(type)
        , color(_color)
        , other(_other)
    {
    }

    ColorOperationData(const ColorOperationData &_other)
        : ColorData(_other)
    {
        color = _other.color;
        other = _other.other;
    }

    inline bool equals(const ColorData *_other) const override
    {
        auto otherData = static_cast<const ColorOperationData *>(_other);
        return color == otherData->color && other == otherData->other;
    }

    Color color;
    Color other;
};

struct AddOperationData : ColorOperationData {
    AddOperationData(const Color &_color, const Color &_other)
        : ColorOperationData(Type::AddOperation, _color, _other)
    {
    }

    inline ColorData *toRgba() const override
    {
        auto colorRgba = color.toRgba();
        auto otherRgba = other.toRgba();

        return new RgbaData{
            uint8_t(std::clamp(colorRgba.red() + otherRgba.red(), 0, 255)),
            uint8_t(std::clamp(colorRgba.green() + otherRgba.green(), 0, 255)),
            uint8_t(std::clamp(colorRgba.blue() + otherRgba.blue(), 0, 255)),
            uint8_t(std::clamp(colorRgba.alpha() + otherRgba.alpha(), 0, 255)),
        };
    }

    inline QString toString() const override
    {
        return u"Add, color: %1, other: %2"_s.arg(color.toString(), other.toString());
    }
};

struct SubtractOperationData : ColorOperationData {
    SubtractOperationData(const Color &_color, const Color &_other)
        : ColorOperationData(Type::SubtractOperation, _color, _other)
    {
    }

    inline ColorData *toRgba() const override
    {
        auto colorRgba = color.toRgba();
        auto otherRgba = other.toRgba();

        return new RgbaData{
            uint8_t(std::clamp(colorRgba.red() - otherRgba.red(), 0, 255)),
            uint8_t(std::clamp(colorRgba.green() - otherRgba.green(), 0, 255)),
            uint8_t(std::clamp(colorRgba.blue() - otherRgba.blue(), 0, 255)),
            uint8_t(std::clamp(colorRgba.alpha() - otherRgba.alpha(), 0, 255)),
        };
    }

    inline QString toString() const override
    {
        return u"Subtract, color: %1, other: %2"_s.arg(color.toString(), other.toString());
    }
};

struct MultiplyOperationData : ColorOperationData {
    MultiplyOperationData(const Color &_color, const Color &_other)
        : ColorOperationData(Type::MultiplyOperation, _color, _other)
    {
    }

    inline ColorData *toRgba() const override
    {
        auto colorRgba = color.toRgba();
        auto otherRgba = other.toRgba();

        return new RgbaData{
            uint8_t(std::clamp(int(((colorRgba.red() / 255.0) * (otherRgba.red() / 255.0)) * 255.0), 0, 255)),
            uint8_t(std::clamp(int(((colorRgba.green() / 255.0) * (otherRgba.green() / 255.0)) * 255.0), 0, 255)),
            uint8_t(std::clamp(int(((colorRgba.blue() / 255.0) * (otherRgba.blue() / 255.0)) * 255.0), 0, 255)),
            uint8_t(std::clamp(int(((colorRgba.alpha() / 255.0) * (otherRgba.alpha() / 255.0)) * 255.0), 0, 255)),
        };
    }

    inline QString toString() const override
    {
        return u"Multiply, color: %1, other: %2"_s.arg(color.toString(), other.toString());
    }
};

struct SetOperationData : ColorData {
    SetOperationData(const Color &_color, std::optional<uint8_t> _r, std::optional<uint8_t> _g, std::optional<uint8_t> _b, std::optional<uint8_t> _a)
        : ColorData(Type::SetOperation)
        , color(_color)
        , r(_r)
        , g(_g)
        , b(_b)
        , a(_a)
    {
    }

    SetOperationData(const SetOperationData &other)
        : ColorData(other)
    {
        color = other.color;
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
    }

    ColorData *toRgba() const override
    {
        auto colorRgba = color.toRgba();

        return new RgbaData{
            r.value_or(colorRgba.red()),
            g.value_or(colorRgba.green()),
            b.value_or(colorRgba.blue()),
            a.value_or(colorRgba.alpha()),
        };
    }

    QString toString() const override
    {
        return u"Set, color: %1, r: %2, g: %3, b: %4, a: %5"_s //
            .arg(color.toString())
            .arg(r ? QString::number(r.value()) : u"None"_s)
            .arg(g ? QString::number(g.value()) : u"None"_s)
            .arg(b ? QString::number(b.value()) : u"None"_s)
            .arg(a ? QString::number(a.value()) : u"None"_s);
    }

    bool equals(const ColorData *other) const override
    {
        auto otherData = static_cast<const SetOperationData *>(other);
        return color == otherData->color && r == otherData->r && g == otherData->g && b == otherData->b && a == otherData->a;
    }

    Color color;
    std::optional<uint8_t> r;
    std::optional<uint8_t> g;
    std::optional<uint8_t> b;
    std::optional<uint8_t> a;
};

struct MixOperationData : ColorData {
    MixOperationData(const Color &_color, const Color &_other, qreal _amount)
        : ColorData(Type::MixOperation)
        , color(_color)
        , other(_other)
        , amount(_amount)
    {
    }

    MixOperationData(const MixOperationData &_other)
        : ColorData(_other)
    {
        color = _other.color;
        other = _other.other;
        amount = _other.amount;
    }

    inline ColorData *toRgba() const override
    {
        auto colorRgba = color.toRgba();
        auto otherRgba = other.toRgba();

        auto invAmount = 1.0 - amount;

        return new RgbaData{
            uint8_t(colorRgba.red() * invAmount + otherRgba.red() * amount),
            uint8_t(colorRgba.green() * invAmount + otherRgba.green() * amount),
            uint8_t(colorRgba.blue() * invAmount + otherRgba.blue() * amount),
            uint8_t(colorRgba.alpha() * invAmount + otherRgba.alpha() * amount),
        };
    }

    inline QString toString() const override
    {
        return u"Mix, color: %1, other: %2, amount: %3"_s.arg(color.toString(), other.toString()).arg(amount);
    }

    inline bool equals(const ColorData *_other) const override
    {
        auto otherData = static_cast<const MixOperationData *>(_other);
        return color == otherData->color && other == otherData->other && qFuzzyCompare(amount, otherData->amount);
    }

    Color color;
    Color other;
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

Color::Color(Color &&other)
{
    std::swap(data, other.data);
}

Color &Color::operator=(const Color &other)
{
    data = other.data;
    return *this;
}

Color &Color::operator=(Color &&other)
{
    std::swap(data, other.data);
    return *this;
}

bool Color::operator==(const Color &other) const
{
    if (data == other.data) {
        return true;
    }

    if (!data || !other.data) {
        return false;
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

Color Color::add(const Color &color, const Color &other)
{
    return Color(new AddOperationData(color, other));
}

Color Color::subtract(const Color &color, const Color &other)
{
    return Color(new SubtractOperationData(color, other));
}

Color Color::multiply(const Color &color, const Color &other)
{
    return Color(new MultiplyOperationData(color, other));
}

Color Color::set(const Color &color, std::optional<uint8_t> r, std::optional<uint8_t> g, std::optional<uint8_t> b, std::optional<uint8_t> a)
{
    return Color(new SetOperationData(color, r, g, b, a));
}

Color Color::mix(const Color &color, const Color &other, qreal amount)
{
    return Color(new MixOperationData(color, other, amount));
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
