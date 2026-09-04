// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "CssLoader.h"

#include <QFile>
#include <QMetaEnum>
#include <QRegularExpression>
#include <QStandardPaths>

#include <Color.h>
#include <Style.h>
#include <StyleRule.h>

#include <CssParser.h>

#include "CssHelpers.h"
#include "CssTypes.h"

#include "css_logging.h"

using namespace Qt::StringLiterals;
using namespace std::string_literals;
using namespace CssInput;

namespace fs = std::filesystem;

bool matches_keyword(const cssparser::Value &value, const QString &keyword)
{
    if (value.type() != cssparser::Value::Type::String) {
        return false;
    }

    auto string = QString::fromStdString(value.get<std::string>());
    return string.compare(keyword, Qt::CaseInsensitive) == 0;
}

template<typename T>
inline void setImage(T *output, const fs::path &rootPath, const cssparser::Property &property)
{
    PropertyGroupBuilder image(output, &T::image, &T::setImage);

    fs::path path = rootPath / to_path(property.value());
    if (!fs::exists(path)) {
        qCWarning(UNION_CSS) << "Could not load image" << path.string();
        return;
    }

    image->setSource(path);

    if (property.values().size() > 1) {
        auto value = property.value(1);
        if (matches_keyword(value, u"mask"_s)) {
            image->setFlags(ImageFlag::Mask);
        } else if (matches_keyword(value, u"inverted-mask"_s)) {
            image->setFlags(ImageFlag::InvertedMask);
        }
        image->setMaskColor(to_color(property.value(2)));
    }
}

template<typename T>
inline void setAlignment(T *output, const cssparser::Property &property)
{
    PropertyGroupBuilder alignment(output, &T::alignment, &T::setAlignment);

    /* clang-format off */
    switchString(property
        ,Default{[&]{
            if (property.values().size() == 4) {
                alignment->setContainer(toEnumValue<AlignmentContainer>(property.value(0)));
                alignment->setHorizontal(toEnumValue<Alignment>(property.value(1)));
                alignment->setVertical(toEnumValue<Alignment>(property.value(2)));
                alignment->setOrder(property.value<int>(3));
            }
        }}
        ,Case{EndsWith{"container"}, [&](auto &&value) {
            alignment->setContainer(toEnumValue<AlignmentContainer>(value));
        }}
        ,Case{EndsWith{"horizontal"}, [&](auto &&value) {
            alignment->setHorizontal(toEnumValue<Alignment>(value));
        }}
        ,Case{EndsWith{"vertical"}, [&](auto &&value) {
            alignment->setVertical(toEnumValue<Alignment>(value));
        }}
        ,Case{EndsWith{"order"}, [&](CssValue &&value) {
            alignment->setOrder(value.get<int>());
        }}
    );
    /* clang-format on */
}

inline void sizeFromProperty(SizePropertyGroup *output, const cssparser::Property &property)
{
    /* clang-format off */
    switchString(property
        ,Default{[&] {
            if (property.values().size() == 1) {
                auto value = to_px(property.value());
                output->setLeft(value);
                output->setRight(value);
                output->setTop(value);
                output->setBottom(value);
            } else if (property.values().size() == 2) {
                auto horizontal = to_px(property.value(0));
                auto vertical = to_px(property.value(1));
                output->setLeft(horizontal);
                output->setRight(horizontal);
                output->setTop(vertical);
                output->setBottom(vertical);
            } else if (property.values().size() == 3) {
                output->setTop(to_px(property.value(0)));
                output->setRight(to_px(property.value(1)));
                output->setBottom(to_px(property.value(2)));
                output->setLeft(to_px(property.value(1)));
            } else if (property.values().size() == 4) {
                output->setTop(to_px(property.value(0)));
                output->setRight(to_px(property.value(1)));
                output->setBottom(to_px(property.value(2)));
                output->setLeft(to_px(property.value(3)));
            }
        }}
        ,Case{EndsWith{"left"}, [&](auto &&value) {
            output->setLeft(to_px(value));
        }}
        ,Case{EndsWith{"right"}, [&](auto &&value) {
            output->setRight(to_px(value));
        }}
        ,Case{EndsWith{"top"}, [&](auto &&value) {
            output->setTop(to_px(value));
        }}
        ,Case{EndsWith{"bottom"}, [&](auto &&value) {
            output->setBottom(to_px(value));
        }}
    );
    /* clang-format on */
}

template<typename T>
inline void setDirectionValue(T *output, const std::string &baseName, const cssparser::Property &property)
{
    QList<QByteArray> directions;
    QList<QByteArray> properties;
    QList<cssparser::Value> values;

    auto parts = QByteArray(property.name()).split('-');
    if (parts.isEmpty() || parts.first() != QByteArrayView(baseName)) {
        qCDebug(UNION_CSS) << "Ignoring invalid property assignment of" << parts << "expected" << baseName;
        return;
    }

    if (parts.size() == 1) {
        directions = {"left", "right", "top", "bottom"};
        properties = {"width", "style", "color"};

        if (property.values().size() == 1 && property.value().type() == cssparser::Value::Type::String) {
            values = {cssparser::Value{}, property.value(), cssparser::Value{}};
        } else {
            values = {property.value(0), property.value(1), property.value(2)};
        }
    } else if (parts.size() == 2) {
        auto propertyName = parts.last();
        if (propertyName == "width" || propertyName == "style" || propertyName == "color") {
            directions = {"left", "right", "top", "bottom"};
            properties = {propertyName};
            values = {property.value()};
        } else {
            directions = {propertyName};
            properties = {"width", "style", "color"};

            if (property.values().size() == 1 && matches_keyword(property.value(), u"none"_s)) {
                values = {cssparser::Value{}, property.value(), cssparser::Value{}};
            } else {
                values = {property.value(0), property.value(1), property.value(2)};
            }
        }
    } else {
        directions = {parts.at(1)};
        properties = {parts.at(2)};
        values = {property.value()};
    }

    auto setLineValue = [](auto &&line, const QByteArray &property, const cssparser::Value &value) {
        if (property == "width" || property == "size") {
            line->setSize(to_px(value));
        } else if (property == "style") {
            line->setStyle(toEnumValue<Union::Properties::LineStyle>(value.get<std::string>()));
        } else if (property == "color") {
            line->setColor(to_color(value));
        }
    };

    for (const auto &direction : std::as_const(directions)) {
        for (qsizetype i = 0; i < properties.size(); ++i) {
            auto property = properties.at(i);
            auto value = values.at(i);
            if (direction == "left") {
                setLineValue(PropertyGroupBuilder(output, &T::left, &T::setLeft), property, value);
            } else if (direction == "right") {
                setLineValue(PropertyGroupBuilder(output, &T::right, &T::setRight), property, value);
            } else if (direction == "top") {
                setLineValue(PropertyGroupBuilder(output, &T::top, &T::setTop), property, value);
            } else if (direction == "bottom") {
                setLineValue(PropertyGroupBuilder(output, &T::bottom, &T::setBottom), property, value);
            }
        }
    }
}

bool CssLoader::load(Style::Ptr style)
{
    m_stylePath = style->path() / "contents" / "css" / "style.css";

    cssparser::StyleSheet styleSheet(m_stylePath);

    auto defaultsPath =
        fs::path(QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"union/css/defaults/default.css"_s, QStandardPaths::LocateFile).toStdString());
    styleSheet.import(defaultsPath);
    styleSheet.parse();

    const auto paths = styleSheet.paths();
    for (const auto &path : paths) {
        style->addCachePath(path);
    }

    if (styleSheet.errors().size() > 0) {
        style->setHasErrors(true);

        qCWarning(UNION_CSS) << "Errors encountered while parsing CSS:";
        for (const auto &error : styleSheet.errors()) {
            qCWarning(UNION_CSS) << "In file" << error.file << "on line" << error.line << "column" << error.column << error.message.data();
        }
    }

    for (const auto &rule : styleSheet.rules()) {
        if (rule.properties().empty()) {
            continue;
        }

        auto styleRule = StyleRule::create();
        styleRule->setSelectors(createSelectorList(rule.selector()));

        auto properties = std::make_unique<StylePropertyGroup>();
        createProperties(properties.get(), rule.properties());
        styleRule->setProperties(std::move(properties));
        style->insert(styleRule);
    }

    return true;
}

Union::SelectorList CssLoader::createSelectorList(const cssparser::Selector &selector)
{
    Union::SelectorList result;
    std::ranges::transform(selector.parts(), std::back_inserter(result), [this](const auto &part) {
        return createSelector(part);
    });
    return result;
}

Union::Selector CssLoader::createSelector(const cssparser::SelectorPart &part)
{
    switch (part.kind()) {
    case cssparser::SelectorPart::Kind::Unknown:
    case cssparser::SelectorPart::Kind::RelativeParent:
    case cssparser::SelectorPart::Kind::CombinatorStart:
        // Ignore
        break;
    case cssparser::SelectorPart::Kind::AnyElement:
        return Union::Selector::create<Union::SelectorType::AnyElement>();
    case cssparser::SelectorPart::Kind::Type:
        return Union::Selector::create<Union::SelectorType::Type>(QString::fromStdString(part.value().get<std::string>()));
    case cssparser::SelectorPart::Kind::Id:
        return Union::Selector::create<Union::SelectorType::Id>(QString::fromStdString(part.value().get<std::string>()));
    case cssparser::SelectorPart::Kind::PseudoClass: {
        auto value = toEnumIntValue<Union::Element::States>(part.value().get<std::string>());
        return Union::Selector::create<Union::SelectorType::State>(Union::Element::State{value});
    }
    case cssparser::SelectorPart::Kind::Class:
        return Union::Selector::create<Union::SelectorType::Hint>(QString::fromStdString(part.value().get<std::string>()));
    case cssparser::SelectorPart::Kind::Attribute: {
        auto attributeMatch = part.attributeMatch().value();
        auto name = QString::fromStdString(attributeMatch.name());
        switch (attributeMatch.op()) {
        case cssparser::AttributeMatch::Operator::Exists:
            return Union::Selector::create<Union::SelectorType::AttributeExists>(name);
        case cssparser::AttributeMatch::Operator::Equals:
            return Union::Selector::create<Union::SelectorType::AttributeEquals>(std::make_pair(name, to_qvariant(attributeMatch.value())));
        case cssparser::AttributeMatch::Operator::Includes:
        case cssparser::AttributeMatch::Operator::Prefixed:
        case cssparser::AttributeMatch::Operator::Suffixed:
            // TODO The above should technically not be substring matches
        case cssparser::AttributeMatch::Operator::Substring: {
            auto value = QString::fromStdString(attributeMatch.value().get<std::string>());
            return Union::Selector::create<Union::SelectorType::AttributeSubstringMatch>(std::make_pair(name, value));
        }
        case cssparser::AttributeMatch::Operator::DashMatch:
            // TODO
        case cssparser::AttributeMatch::Operator::None:
            break;
        }

        break;
    }
    case cssparser::SelectorPart::Kind::DocumentRoot:
        // TODO
        break;
    case cssparser::SelectorPart::Kind::DescendantCombinator:
        return Union::Selector::create<Union::SelectorType::DescendantCombinator>();
    case cssparser::SelectorPart::Kind::ChildCombinator:
        return Union::Selector::create<Union::SelectorType::ChildCombinator>();
    }

    return Union::Selector::create();
}

void CssLoader::createProperties(StylePropertyGroup *output, std::span<const cssparser::Property> properties)
{
    /* clang-format off */
    for (const auto &property : properties) {
        switchString(property
            ,Case{{"visibility", u"opacity"}, [&]{
                setDisplayProperty(output, property);
            }}
            ,Case{{"width", "height", "spacing"}, [&]{
                setLayoutProperty(output, property);
            }}
            ,Case{StartsWith{"padding"}, [&]{
                setLayoutProperty(output, property);
            }}
            ,Case{StartsWith{"inset"}, [&]{
                setLayoutProperty(output, property);
            }}
            ,Case{StartsWith{"margin"}, [&]{
                setLayoutProperty(output, property);
            }}
            ,Case{StartsWith{"layout"}, [&]{
                setLayoutProperty(output, property);
            }}
            ,Case{StartsWith{"background"}, [&]{
                setBackgroundProperty(output, property);
            }}
            ,Case{StartsWith{"border"}, [&]{
                setBorderProperty(output, property);
            }}
            ,Case{StartsWith{"outline"}, [&]{
                setOutlineProperty(output, property);
            }}
            ,Case{StartsWith{"text"}, [&]{
                setTextProperty(output, property);
            }}
            ,Case{StartsWith{"font"}, [&]{
                setTextProperty(output, property);
            }}
            ,Case{StartsWith{"icon"}, [&]{
                setIconProperty(output, property);
            }}
            ,Case{"color", [&]{
                setTextProperty(output, property);
                setIconProperty(output, property);
            }}
            ,Case{StartsWith{"shadow"}, [&]{
                setShadowProperty(output, property);
            }}
            ,Case{StartsWith{"box-shadow"}, [&]{
                setShadowProperty(output, property);
            }}
        );
    }
    /* clang-format on */
}

void CssLoader::setDisplayProperty(StylePropertyGroup *output, const cssparser::Property &property)
{
    PropertyGroupBuilder display(output, &StylePropertyGroup::display, &StylePropertyGroup::setDisplay);

    /* clang-format off */
    switchString(property
        ,Case{"visibility", [&](auto &&value) {
            switchString(value
                ,Case{"visible", [&]{ display->setVisible(true); }}
                ,Case{"hidden", [&]{ display->setVisible(false); }}
            );
        }}
        ,Case{"opacity", [&](auto &&value) {
            display->setOpacity(to_number(value));
        }}
    );
    /* clang-format on */
}

void CssLoader::setLayoutProperty(StylePropertyGroup *output, const cssparser::Property &property)
{
    PropertyGroupBuilder layout(output, &StylePropertyGroup::layout, &StylePropertyGroup::setLayout);

    /* clang-format off */
    switchString(property
        ,Case{"width", [&](auto &&value) {
            layout->setWidth(to_px(value));
        }}
        ,Case{"height", [&](auto &&value) {
            layout->setHeight(to_px(value));
        }}
        ,Case{"spacing", [&](auto &&value) {
            layout->setSpacing(to_px(value));
        }}
        ,Case{StartsWith{"layout-alignment"}, [&]{
            setAlignment(layout.instance, property);
        }}
        ,Case{StartsWith{"padding"}, [&]{
            PropertyGroupBuilder padding(layout.instance, &LayoutPropertyGroup::padding, &LayoutPropertyGroup::setPadding);
            sizeFromProperty(padding.instance, property);
        }}
        ,Case{StartsWith{"inset"}, [&]{
            PropertyGroupBuilder inset(layout.instance, &LayoutPropertyGroup::inset, &LayoutPropertyGroup::setInset);
            sizeFromProperty(inset.instance, property);
        }}
        ,Case{StartsWith{"margin"}, [&]{
            PropertyGroupBuilder margins(layout.instance, &LayoutPropertyGroup::margins, &LayoutPropertyGroup::setMargins);
            sizeFromProperty(margins.instance, property);
        }}
    );
    /* clang-format on */
}

void CssLoader::setBackgroundProperty(StylePropertyGroup *output, const cssparser::Property &property)
{
    PropertyGroupBuilder background(output, &StylePropertyGroup::background, &StylePropertyGroup::setBackground);

    /* clang-format off */
    switchString(property
        ,Case{"background", [&](auto &&value) {
            if (matches_keyword(value, u"none"_s)) {
                background->setColor(Union::Color{});
                background->setImage(ImagePropertyGroup::empty());
            } else if (value.type() == cssparser::Value::Type::Color) {
                background->setColor(to_color(value));
            } else if (value.type() == cssparser::Value::Type::Url) {
                setImage(background.instance, m_stylePath, property);
            }
        }}
        ,Case{"background-color", [&](auto &&value) {
            background->setColor(to_color(value));
        }}
        ,Case{"background-image", [&](auto &&value) {
            if (matches_keyword(value, u"none"_s)) {
                background->setImage(ImagePropertyGroup::empty());
            } else {
                setImage(background.instance, m_stylePath, property);
            }
        }}
        ,Case{"background-image-mask-color", [&](auto &&value) {
            PropertyGroupBuilder image(background.instance, &BackgroundPropertyGroup::image, &BackgroundPropertyGroup::setImage);
            image->setMaskColor(to_color(value));
        }}
    );
    /* clang-format on */
}

void CssLoader::setBorderProperty(StylePropertyGroup *output, const cssparser::Property &property)
{
    PropertyGroupBuilder border(output, &StylePropertyGroup::border, &StylePropertyGroup::setBorder);

    if (property.name().ends_with("radius")) {
        PropertyGroupBuilder corners(output, &StylePropertyGroup::corners, &StylePropertyGroup::setCorners);

        auto setCornerRadius = [](auto &&corner, qreal radius) {
            corner->setRadius(radius);
        };

        /* clang-format off */
        switchString(property
            ,Default{[&]{
                if (property.values().size() == 1) {
                    auto radius = to_px(property.value());
                    setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::topLeft, &CornersPropertyGroup::setTopLeft), radius);
                    setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::topRight, &CornersPropertyGroup::setTopRight), radius);
                    setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::bottomLeft, &CornersPropertyGroup::setBottomLeft), radius);
                    setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::bottomRight, &CornersPropertyGroup::setBottomRight), radius);
                } else if (property.values().size() == 4) {
                    setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::topLeft, &CornersPropertyGroup::setTopLeft),
                                    to_px(property.value(0)));
                    setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::topRight, &CornersPropertyGroup::setTopRight),
                                    to_px(property.value(1)));
                    setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::bottomRight, &CornersPropertyGroup::setBottomRight),
                                    to_px(property.value(2)));
                    setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::bottomLeft, &CornersPropertyGroup::setBottomLeft),
                                    to_px(property.value(3)));
                }
            }}
            ,Case{"border-top-left-radius", [&](auto &&value) {
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::topLeft, &CornersPropertyGroup::setTopLeft), to_px(value));
            }}
            ,Case{"border-top-right-radius", [&](auto &&value) {
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::topRight, &CornersPropertyGroup::setTopRight), to_px(value));
            }}
            ,Case{"border-bottom-left-radius", [&](auto &&value) {
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::bottomLeft, &CornersPropertyGroup::setBottomLeft), to_px(value));
            }}
            ,Case{"border-bottom-right-radius", [&](auto &&value) {
                setCornerRadius(PropertyGroupBuilder(corners.instance, &CornersPropertyGroup::bottomRight, &CornersPropertyGroup::setBottomRight), to_px(value));
            }}
        );
        /* clang-format on */

        return;
    }

    setDirectionValue(border.instance, "border"s, property);
}

void CssLoader::setOutlineProperty(StylePropertyGroup *output, const cssparser::Property &property)
{
    PropertyGroupBuilder outline(output, &StylePropertyGroup::outline, &StylePropertyGroup::setOutline);
    setDirectionValue(outline.instance, "outline"s, property);
}

void CssLoader::setTextProperty(StylePropertyGroup *output, const cssparser::Property &property)
{
    PropertyGroupBuilder text(output, &StylePropertyGroup::text, &StylePropertyGroup::setText);

    /* clang-format off */
    switchString(property
        ,Case{StartsWith{"text-alignment"}, [&] {
            setAlignment(text.instance, property);
        }}
        ,Case{StartsWith{"font"}, [&]{
            auto font = text->font().value_or(QFont{});

            switchString(property
                ,Case{"font-familty", [&](auto &&value) {
                    font.setFamily(QString::fromStdString(property.value<std::string>()));
                }}
                ,Case{"font-size", [&](CssValue &&value) {
                    auto dimension = value.get<cssparser::Dimension>();
                    switch (dimension.unit()) {
                        case cssparser::Dimension::Unit::Px:
                            font.setPixelSize(int(dimension.value()));
                            break;
                        case cssparser::Dimension::Unit::Pt:
                            font.setPointSizeF(dimension.value());
                            break;
                        case cssparser::Dimension::Unit::Percent:
                            font.setPointSizeF(font.pointSizeF() * dimension.value());
                            break;
                        default:
                            qCWarning(UNION_CSS) << "Invalid unit for font-size";
                            break;
                    }
                }}
                ,Case{"font-weight", [&](CssValue &&value) {
                    if (value.type() == cssparser::Value::Type::Integer) {
                        font.setWeight(QFont::Weight(value.get<int>()));
                    } else {
                        switchString(value
                            ,Case{"normal", [&]{ font.setWeight(QFont::Weight::Normal); }}
                            ,Case{"bold", [&]{ font.setWeight(QFont::Weight::Bold); }}
                            ,Case{"bolder", [&]{ font.setWeight(QFont::Weight(font.weight() + 100)); }}
                            ,Case{"lighter", [&]{ font.setWeight(QFont::Weight(font.weight() - 100); }}
                        );
                    }
                }}
            );

            text->setFont(font);
        }}
        ,Case{{"color", "text-color"}, [&](auto &&value) {
            text->setColor(to_color(value));
        }}
        ,Case{"text-wrap-mode", [&](auto &&value) {
            // Wrap is shorthand for WrapAtWordBoundaryOrAnywhere
            if (matches_keyword(value, "wrap")) {
                text->setWrapMode(TextWrapMode::WrapAtWordBoundaryOrAnywhere);
            } else {
                text->setWrapMode(toEnumValue<TextWrapMode>(property.value<std::string>()));
            }
        }}
        ,Case{"text-elide", [&](auto &&value) {
            text->setElide(toEnumValue<TextElide>(value));
        }}
    );
    /* clang-format on */
}

void CssLoader::setIconProperty(StylePropertyGroup *output, const cssparser::Property &property)
{
    PropertyGroupBuilder icon(output, &StylePropertyGroup::icon, &StylePropertyGroup::setIcon);

    /* clang-format off */
    switchString(property
        ,Case{StartsWith{"icon-alignment"}, [&]{
            setAlignment(icon.instance, property);
        }}
        ,Case{u"icon-width"_s, [&](auto &&value) {
            icon->setWidth(to_px(value));
        }}
        ,Case{u"icon-height"_s, [&](auto &&value) {
            icon->setHeight(to_px(value));
        }}
        ,Case{u"icon-size"_s, [&](auto &&value) {
            icon->setWidth(to_px(value));
            icon->setHeight(to_px(value));
        }}
        ,Case{u"icon-name"_s, [&](auto &&value) {
            icon->setName(to_string(value));
        }}
        ,Case{u"icon-source"_s, [&](auto &&value) {
            icon->setSource(QUrl{to_string(value)});
        }}
        ,Case{u"color"_s, [&](auto &&value) {
            icon->setColor(to_color(value));
        }}
        ,Case{u"icon-color"_s, [&](auto &&value) {
            icon->setColor(to_color(value));
        }}
    );
    /* clang-format on */
}

void CssLoader::setShadowProperty(StylePropertyGroup *output, const cssparser::Property &property)
{
    if (property.name() == "box-shadow") {
        if (matches_keyword(property.value(), u"none"_s)) {
            output->setShadow(ShadowPropertyGroup::empty());
        } else {
            PropertyGroupBuilder shadow(output, &StylePropertyGroup::shadow, &StylePropertyGroup::setShadow);

            PropertyGroupBuilder offset(shadow.instance, &ShadowPropertyGroup::offset, &ShadowPropertyGroup::setOffset);
            offset->setHorizontal(to_px(property.value(0)));
            offset->setVertical(to_px(property.value(1)));

            shadow->setBlur(to_px(property.value(2)));
            shadow->setSize(to_px(property.value(3)));
            shadow->setColor(to_color(property.value(4)));
        }
    }
}
