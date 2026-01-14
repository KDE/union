// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#pragma once

#include <memory>

#include <QColor>
#include <QSharedData>
#include <QString>
#include <QVariant>

#include "union_export.h"

namespace Union
{

// Private data for Color
struct ColorData : public QSharedData {
    enum class Type {
        Empty,
        RGBA,
        Custom,
        AddOperation,
        SubtractOperation,
        MultiplyOperation,
        SetOperation,
        MixOperation,
    };

    ColorData(Type _type)
        : QSharedData()
        , type(_type)
    {
    }

    ColorData(const ColorData &other)
        : QSharedData(other)
        , type(other.type)
    {
    }

    virtual ~ColorData()
    {
    }

    virtual ColorData *toRgba() const = 0;
    virtual QString toString() const = 0;
    virtual bool equals(const ColorData *other) const = 0;

    const Type type = Type::Empty;
};

/*!
 * \class Union::Color
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief A class that represents a color that can be provided from different sources.
 *
 * The main purpose of this class is to store information about a color without
 * needing to resolve said color to an actual concrete RGBA value. This allows
 * input plugins to provide colors that can be cached without having to
 * regenerate the cache if some color changes.
 */
class UNION_EXPORT Color
{
public:
    /*!
     * Constructor.
     *
     * Constructs an invalid color.
     */
    Color();

    Color(const Color &other);

    Color(Color &&other);

    Color &operator=(const Color &other);

    Color &operator=(Color &&other);

    bool operator==(const Color &other) const;

    /*!
     * Returns if this color has any data.
     */
    bool isValid() const;

    /*!
     * Returns the type of color.
     */
    ColorData::Type type() const;

    /*!
     * Returns the red component of this color if this is an RGBA color, otherwise returns 0.
     */
    uint8_t red() const;
    /*!
     * Returns the green component of this color if this is an RGBA color, otherwise returns 0.
     */
    uint8_t green() const;
    /*!
     * Returns the blue component of this color if this is an RGBA color, otherwise returns 0.
     */
    uint8_t blue() const;
    /*!
     * Returns the alpha component of this color if this is an RGBA color, otherwise returns 0.
     */
    uint8_t alpha() const;

    /*!
     * Returns this color converted to an RGBA color.
     *
     * How the exact conversion happens depends on the current color type.
     */
    Color toRgba() const;

    /*!
     * Returns this color converted to a QColor.
     *
     * If this color is not of RGBA type, it is first converted to RGBA before
     * converting to QColor.
     */
    QColor toQColor() const;

    /*!
     * Returns a string representation of this color.
     */
    QString toString() const;

    /*!
     * Return a new RGBA color.
     */
    static Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    /*!
     * Return a new custom color.
     *
     * \p source is the custom color source, which needs to be the name of a
     * ColorProvider. \p arguments is a list of arguments to supply to the
     * specified ColorProvider.
     */
    static Color custom(const QString &source, const QStringList &arguments);
    /*!
     * Return a new add color.
     *
     * Add colors represents the addition of color \p other to \p color.
     */
    static Color add(const Color &color, const Color &other);
    /*!
     * Return a new subtract color.
     *
     * Subtract colors represents the subtraction of color \p other from \p color.
     */
    static Color subtract(const Color &color, const Color &other);
    /*!
     * Return a new multiply color.
     *
     * Multiply colors represents the multiplication of color \p color by \p other.
     */
    static Color multiply(const Color &color, const Color &other);
    /*!
     * Return a new set color.
     *
     * Set colors represents a color operation where the values of certain
     * components of color \p color are replaced with different values.
     */
    static Color set(const Color &color, std::optional<uint8_t> r, std::optional<uint8_t> g, std::optional<uint8_t> b, std::optional<uint8_t> a);
    /*!
     * Return a new mix color.
     *
     * Mix colors represent the linear interpolation between the colors \p first
     * and \p second with the specified \p amount.
     */
    static Color mix(const Color &first, const Color &second, qreal amount);

private:
    Color(ColorData *_data);

    QSharedDataPointer<ColorData> data;
};

/*!
 * \class ColorProvider
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief An interface for objects that can provide custom colors.
 */
class UNION_EXPORT ColorProvider : public QObject
{
    Q_OBJECT
public:
    struct Rgba {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 0;
    };

    using QObject::QObject;

    /*!
     * Returns a color matching the provided arguments.
     *
     * The arguments depend on the actual provider implementation.
     */
    virtual std::optional<Rgba> color(const QStringList &arguments) const = 0;

    /*!
     * Register a color provider.
     */
    static void registerProvider(const QString &name, ColorProvider *provider);
};

}

Q_DECLARE_INTERFACE(Union::ColorProvider, "org.kde.union.ColorProvider")
Q_DECLARE_METATYPE(Union::ColorProvider)

UNION_EXPORT QDebug &operator<<(QDebug &stream, const Union::Color &color);

/*!
 * \relates Union::Color
 *
 * Specialization of std::formatter to support formatting an element as string.
 */
template<>
struct std::formatter<Union::Color, char> : public std::formatter<std::string, char> {
    template<class FormatContext>
    FormatContext::iterator format(const Union::Color &value, FormatContext &context) const
    {
        auto string = value.toString().toStdString();
        return std::formatter<std::string, char>::format(string, context);
    }
};

Q_DECLARE_METATYPE(Union::Color)
