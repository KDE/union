/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "PlasmaSvgLoader.h"
#include "RYMLHelpers.h"

#include <unordered_set>

#include <QDir>
#include <QFile>
#include <QIcon>
#include <QPainter>
#include <QStack>

#include <KConfigGroup>
#include <Plasma/Theme>

#include <StyleRule.h>
#include <Theme.h>

#include "LoadingContext.h"
#include "PlasmaSvgRenderer.h"
#include "PropertyFunctions.h"

#include "plasmasvg_logging.h"

using namespace Union;
using namespace Union::Properties;
using namespace Qt::StringLiterals;
using namespace std::string_literals;

constexpr char16_t PluginName[] = u"plasmasvg";

void logError(ryml::ConstNodeRef node, QByteArrayView property, QByteArrayView value, const PropertyFunctions::Error &error, const LoadingContext &context)
{
    context.logLocation(node).nospace() << "Failed setting value " << value << " of property " << property << ": " << qPrintable(error.message);
}

template<typename T>
std::optional<T> readPropertyValue(QByteArrayView valueName, ryml::ConstNodeRef node, LoadingContext &context)
{
    auto value = PropertyFunctions::elementProperty<T>(node, context);
    if (value.has_value()) {
        return value.value();
    } else {
        logError(node, context.propertyName(), valueName, value.error(), context);
        return std::nullopt;
    }
}

template<typename T>
std::optional<T> emptyOrNull(ryml::ConstNodeRef node)
{
    if (value<QByteArrayView>(node) == "empty") {
        return T::empty();
    }

    return std::nullopt;
}

bool PlasmaSvgLoader::load(Theme::Ptr theme)
{
    QDir dir(u":/org/kde/union/input/plasmasvg"_s);
    const auto entries = dir.entryList({u"*.yml"_s});
    if (entries.isEmpty()) {
        qCWarning(UNION_PLASMASVG) << "No style definitions found";
        return false;
    }

    auto callbacks = ryml::get_callbacks();
    callbacks.m_error = rymlError;
    ryml::set_callbacks(callbacks);

    ryml::ParserOptions options;
    options.locations(true);
    ryml::EventHandlerTree eventHandler;
    ryml::Parser parser{&eventHandler, options};

    for (auto entry : entries) {
        auto path = dir.absoluteFilePath(entry).toStdString();
        QFile file{QString::fromStdString(path)};
        if (!file.open(QFile::ReadOnly)) {
            qCWarning(UNION_PLASMASVG) << "Could not read file" << entry << file.errorString();
            continue;
        }

        auto data = file.readAll().toStdString();
        ryml::Tree parsed;
        try {
            parsed = ryml::parse_in_place(&parser, ryml::to_csubstr(path), ryml::to_substr(data));
        } catch (const std::runtime_error &e) {
            qCWarning(UNION_PLASMASVG) << "Could not parse" << entry << e.what();
            continue;
        }

        parsed.resolve();
        auto root = parsed.crootref();

        if (!root.is_map()) {
            qCWarning(UNION_PLASMASVG) << "Invalid structure in" << entry;
            continue;
        }

        LoadingContext context(theme, &parser);
        createStyles(root, context);
    }

    PlasmaSvgRenderer::clearRenderers(theme);

    return true;
}

const Plasma::Theme *PlasmaSvgLoader::plasmaTheme() const
{
    return &m_plasmaTheme;
}

void PlasmaSvgLoader::createStyles(ryml::ConstNodeRef node, LoadingContext &context)
{
    for (auto child : node.children()) {
        if (!child.is_map()) {
            continue;
        }

        auto selectors = child.find_child("selectors");
        if (!selectors.readable()) {
            context.logLocation(child) << "Ignoring rule with no selectors declared";
            continue;
        }

        if (!selectors.is_seq()) {
            context.logLocation(selectors) << "Key 'selectors' should be a list";
            continue;
        }

        for (auto entry : selectors.children()) {
            auto selectors = createSelectors(entry, context);
            if (selectors.isEmpty()) {
                context.logLocation(entry) << "Ignoring empty selector";
                continue;
            }
            auto style = createStyle(selectors, child, context);
            if (style) {
                context.theme->insert(style);
            }
        }
    }
}

SelectorList PlasmaSvgLoader::createSelectors(ryml::ConstNodeRef node, LoadingContext &context)
{
    SelectorList selectors;

    with_child(node, "type", [&](auto node) {
        selectors.append(Selector::create<SelectorType::Type>(value<QString>(node)));
    });

    with_child(node, "id", [&](auto node) {
        selectors.append(Selector::create<SelectorType::Id>(value<QString>(node)));
    });

    with_child(node, "state", [&](auto node) {
        // Support value or list
        if (node.has_val()) {
            auto val = Element::State(value<Element::States>(node).toInt());
            selectors.append(Selector::create<SelectorType::State>(val));
        } else if (node.is_seq() && node.has_children()) {
            SelectorList allOfList;
            for (auto child : node.children()) {
                if (!child.has_val()) {
                    continue;
                }
                auto val = Element::State(value<Element::States>(child).toInt());
                allOfList.append(Selector::create<SelectorType::State>(val));
            }
            selectors.appendAllOf(allOfList);
        }
    });

    with_child(node, "hints", [&](auto node) {
        // Support value or list
        if (node.has_val()) {
            selectors.append(Selector::create<SelectorType::Hint>(value<QString>(node)));
        } else if (node.is_seq() && node.has_children()) {
            SelectorList allOfList;
            for (auto child : node.children()) {
                if (!child.has_val()) {
                    continue;
                }
                allOfList.append(Selector::create<SelectorType::Hint>(value<QString>(child)));
            }
            selectors.appendAllOf(allOfList);
        }
    });

    with_child(node, "attributes", [&](auto node) {
        if (node.has_children()) {
            SelectorList allOfList;
            for (auto child : node.children()) {
                if (!child.is_keyval()) {
                    continue;
                }
                auto pair = std::make_pair<QString, QVariant>(QString::fromLatin1(child.key()), QString::fromLatin1(child.val()));
                allOfList.append(Selector::create<SelectorType::Attribute>(pair));
            }
            selectors.appendAllOf(allOfList);
        }
    });

    with_child(node, "colorSet", [&](auto node) {
        // Support value or list
        if (node.has_val()) {
            selectors.append(Selector::create<SelectorType::ColorSet>(value<Element::ColorSet>(node)));
        } else if (node.is_seq() && node.has_children()) {
            // You can't display multiple color sets at once,
            // but you could show the same appearance for multiple color sets.
            SelectorList anyOfList;
            for (auto child : node.children()) {
                if (!child.has_val()) {
                    continue;
                }
                anyOfList.append(Selector::create<SelectorType::ColorSet>(value<Element::ColorSet>(child)));
            }
            selectors.appendAnyOf(anyOfList);
        }
    });

    SelectorList childSelectors;
    with_child(node, "child", [&](auto node) {
        if (!node.is_map()) {
            context.logLocation(node) << "Child selector specified but child is not a map, ignoring";
            return;
        }

        childSelectors = createSelectors(node, context);
    });

    SelectorList result;
    result.appendAllOf(selectors);

    if (!childSelectors.isEmpty()) {
        result.append(childSelectors);
    }

    return result;
}

StyleRule::Ptr PlasmaSvgLoader::createStyle(const SelectorList &selectors, ryml::ConstNodeRef node, LoadingContext &context)
{
    auto style = StyleRule::create();
    style->setSelectors(selectors);

    auto cleanup = context.pushFromNode(node, selectors);

    StyleProperty properties;

    with_child(node, "layout", [&](auto node) {
        properties.setLayout(createLayoutProperty(node, context));
    });

    with_child(node, "text", [&](auto node) {
        properties.setText(createTextProperty(node, context));
    });

    with_child(node, "icon", [&](auto node) {
        properties.setIcon(createIconProperty(node, context));
    });

    with_child(node, "background", [&](auto node) {
        properties.setBackground(createBackgroundProperty(node, context));
    });

    properties.setPalette(createPaletteProperty(context));

    with_child(node, "debug", [&](auto node) {
        if (value<bool>(node)) {
            qCDebug(UNION_PLASMASVG) << properties;
        }
    });

    if (properties.hasAnyValue()) {
        style->setProperties(properties);
    }

    with_child(node, "children", [&](auto node) {
        createStyles(node, context);
    });

    return style;
}

std::optional<LayoutProperty> PlasmaSvgLoader::createLayoutProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node, "layout");

    LayoutProperty layout;

    with_child(node, "width", [&](auto node) {
        layout.setWidth(readPropertyValue<qreal>("width", node, context));
    });
    with_child(node, "height", [&](auto node) {
        layout.setHeight(readPropertyValue<qreal>("height", node, context));
    });
    with_child(node, "spacing", [&](auto node) {
        layout.setSpacing(readPropertyValue<qreal>("spacing", node, context));
    });
    with_child(node, "alignment", [&](auto node) {
        layout.setAlignment(createAlignmentProperty(node, context));
    });
    with_child(node, "padding", [&](auto node) {
        layout.setPadding(createSizeProperty(node, context));
    });
    with_child(node, "inset", [&](auto node) {
        layout.setInset(createSizeProperty(node, context));
    });
    with_child(node, "margins", [&](auto node) {
        layout.setMargins(createSizeProperty(node, context));
    });

    return layout;
}

std::optional<TextProperty> PlasmaSvgLoader::createTextProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return emptyOrNull<TextProperty>(node);
    }

    auto cleanup = context.pushFromNode(node, "text");

    TextProperty text;
    with_child(node, "alignment", [&](auto node) {
        text.setAlignment(createAlignmentProperty(node, context));
    });
    with_child(node, "font", [&](auto node) {
        text.setFont(readPropertyValue<QFont>("font", node, context));
    });

    return text;
}

std::optional<IconProperty> PlasmaSvgLoader::createIconProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return emptyOrNull<IconProperty>(node);
    }

    auto cleanup = context.pushFromNode(node, "icon");

    IconProperty icon;
    with_child(node, "alignment", [&](auto node) {
        icon.setAlignment(createAlignmentProperty(node, context));
    });
    with_child(node, "width", [&](auto node) {
        icon.setWidth(readPropertyValue<qreal>("width", node, context));
    });
    with_child(node, "height", [&](auto node) {
        icon.setHeight(readPropertyValue<qreal>("height", node, context));
    });
    with_child(node, "name", [&](auto node) {
        icon.setName(readPropertyValue<QString>("name", node, context));
    });
    with_child(node, "source", [&](auto node) {
        icon.setSource(readPropertyValue<QUrl>("source", node, context));
    });

    return icon;
}

std::optional<BackgroundProperty> PlasmaSvgLoader::createBackgroundProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return emptyOrNull<BackgroundProperty>(node);
    }

    auto cleanup = context.pushFromNode(node, "background");

    BackgroundProperty background;
    with_child(node, "color", [&](auto node) {
        background.setColor(readPropertyValue<QColor>("color", node, context));
    });

    with_child(node, "image", [&](auto node) {
        background.setImage(createImageProperty(node, context));
    });

    with_child(node, "border", [&](auto node) {
        background.setBorder(createBorderProperty(node, context));
    });

    with_child(node, "corners", [&](auto node) {
        background.setCorners(createCornersProperty(node, context));
    });

    with_child(node, "shadow", [&](auto node) {
        background.setShadow(createShadowProperty(node, context));
    });

    return background;
}

std::optional<Union::Properties::PaletteProperty> PlasmaSvgLoader::createPaletteProperty(LoadingContext &context)
{
    if (context.data.colorGroups.isEmpty() && context.data.colorSets.isEmpty()) {
        return std::nullopt;
    }

    Union::Properties::PaletteProperty palette;

    auto colorSet = context.colorSet();
    if (colorSet == Union::Element::ColorSet::None) {
        colorSet = Union::Element::ColorSet::Window;
    }

    KColorScheme colorScheme{context.colorGroup(), static_cast<KColorScheme::ColorSet>(static_cast<int>(colorSet) - 1)};
    palette.setAccent(colorScheme.decoration(KColorScheme::HoverColor).color());
    palette.setAlternateBase(colorScheme.background(KColorScheme::AlternateBackground).color());
    palette.setBase(colorScheme.background(KColorScheme::NormalBackground).color());
    palette.setBrightText(colorScheme.foreground(KColorScheme::ActiveText).color());
    palette.setButton(colorScheme.background(KColorScheme::NormalBackground).color());
    palette.setButtonText(colorScheme.foreground(KColorScheme::NormalText).color());
    palette.setHighlight(colorScheme.background(KColorScheme::ActiveBackground).color());
    palette.setHighlightedText(colorScheme.foreground(KColorScheme::ActiveText).color());
    palette.setLink(colorScheme.foreground(KColorScheme::LinkText).color());
    palette.setText(colorScheme.foreground(KColorScheme::NormalText).color());
    palette.setWindow(colorScheme.background(KColorScheme::NormalBackground).color());
    palette.setWindowText(colorScheme.foreground(KColorScheme::NormalText).color());

    return palette;
}

std::optional<AlignmentProperty> PlasmaSvgLoader::createAlignmentProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return emptyOrNull<AlignmentProperty>(node);
    }

    auto cleanup = context.pushFromNode(node, "alignment");

    Union::Properties::AlignmentProperty alignment;
    with_child(node, "container", [&](auto node) {
        alignment.setContainer(readPropertyValue<Union::Properties::AlignmentContainer>("container", node, context));
    });
    with_child(node, "horizontal", [&](auto node) {
        alignment.setHorizontal(readPropertyValue<Union::Properties::Alignment>("horizontal", node, context));
    });
    with_child(node, "vertical", [&](auto node) {
        alignment.setVertical(readPropertyValue<Union::Properties::Alignment>("vertical", node, context));
    });
    with_child(node, "order", [&](auto node) {
        alignment.setOrder(readPropertyValue<int>("order", node, context));
    });
    return alignment;
}

std::optional<SizeProperty> PlasmaSvgLoader::createSizeProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return emptyOrNull<SizeProperty>(node);
    }

    auto cleanup = context.pushFromNode(node, "size");

    SizeProperty sizes;
    forEachEntry({"left"s, "right"s, "top"s, "bottom"s},
                 sizes,
                 {&SizeProperty::setLeft, &SizeProperty::setRight, &SizeProperty::setTop, &SizeProperty::setBottom},
                 node,
                 [&context](auto node) {
                     auto cleanup = context.pushFromNode(node);
                     return readPropertyValue<qreal>(node.key(), node, context);
                 });

    return sizes;
}

std::optional<BorderProperty> PlasmaSvgLoader::createBorderProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return emptyOrNull<BorderProperty>(node);
    }

    auto cleanup = context.pushFromNode(node);

    BorderProperty border;
    forEachEntry({"left"s, "right"s, "top"s, "bottom"s},
                 border,
                 {&BorderProperty::setLeft, &BorderProperty::setRight, &BorderProperty::setTop, &BorderProperty::setBottom},
                 node,
                 [this, &context](auto node) {
                     auto cleanup = context.pushFromNode(node);
                     return createLineProperty(node, context);
                 });

    return border;
}

std::optional<CornersProperty> PlasmaSvgLoader::createCornersProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return emptyOrNull<CornersProperty>(node);
    }

    auto cleanup = context.pushFromNode(node);

    CornersProperty corners;
    forEachEntry({"top-left"s, "top-right"s, "bottom-left"s, "bottom-right"s},
                 corners,
                 {&CornersProperty::setTopLeft, &CornersProperty::setTopRight, &CornersProperty::setBottomLeft, &CornersProperty::setBottomRight},
                 node,
                 [this, &context](auto node) {
                     auto cleanup = context.pushFromNode(node);
                     return createCornerProperty(node, context);
                 });

    return corners;
}

std::optional<LineProperty> PlasmaSvgLoader::createLineProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return emptyOrNull<LineProperty>(node);
    }

    auto cleanup = context.pushFromNode(node, "line");

    LineProperty line;
    with_child(node, "size", [&](auto node) {
        line.setSize(readPropertyValue<qreal>("size", node, context));
    });
    with_child(node, "image", [&](auto node) {
        line.setImage(createImageProperty(node, context));
    });
    return line;
}

std::optional<CornerProperty> PlasmaSvgLoader::createCornerProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return emptyOrNull<CornerProperty>(node);
    }

    auto cleanup = context.pushFromNode(node);

    CornerProperty corner;
    with_child(node, "image", [&](auto node) {
        corner.setImage(createImageProperty(node, context));
    });
    with_child(node, "size", [&](auto node) {
        auto size = readPropertyValue<QSizeF>("size", node, context);
        if (size.has_value()) {
            corner.setWidth(size.value().width());
            corner.setHeight(size.value().height());
        }
    });
    return corner;
}

std::optional<ImageProperty> PlasmaSvgLoader::createImageProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return emptyOrNull<ImageProperty>(node);
    }

    auto cleanup = context.pushFromNode(node, "image");

    auto data = readPropertyValue<QImage>("imageData", node, context);
    if (!data.has_value()) {
        return std::nullopt;
    }

    ImageProperty image;
    image.setImageData(data.value());
    image.setWidth(data.value().width());
    image.setHeight(data.value().height());
    image.setFlags(Union::Properties::ImageFlag::RepeatBoth);
    return image;
}

std::optional<ShadowProperty> PlasmaSvgLoader::createShadowProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return emptyOrNull<ShadowProperty>(node);
    }

    auto cleanup = context.pushFromNode(node);

    ShadowProperty shadow;
    with_child(node, "offsets", [&](auto node) {
        shadow.setOffsets(createSizeProperty(node, context));
    });

    forEachEntry({"left", "right", "top", "bottom"},
                 shadow,
                 {&ShadowProperty::setLeft, &ShadowProperty::setRight, &ShadowProperty::setTop, &ShadowProperty::setBottom},
                 node,
                 [this, &context](auto node) {
                     auto cleanup = context.pushFromNode(node);
                     return createLineProperty(node, context);
                 });

    forEachEntry({"top-left", "top-right", "bottom-left", "bottom-right"},
                 shadow,
                 {&ShadowProperty::setTopLeft, &ShadowProperty::setTopRight, &ShadowProperty::setBottomLeft, &ShadowProperty::setBottomRight},
                 node,
                 [this, &context](auto node) {
                     auto cleanup = context.pushFromNode(node);
                     return createCornerProperty(node, context);
                 });

    return shadow;
}
