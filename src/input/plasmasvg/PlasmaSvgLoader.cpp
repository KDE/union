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

QFont fontFromName(c4::csubstr name)
{
    auto config = KSharedConfig::openConfig(u"kdeglobals"_s);
    auto group = config->group(u"General"_s);

    QFont font;

    if (name == "system-normal") {
        font = group.readEntry("font", QFont());
    } else if (name == "system-fixed") {
        font = group.readEntry("fixed", QFont());
    } else if (name == "system-small") {
        font = group.readEntry("smallestReadableFont", QFont());
    } else if (name == "system-toolbar") {
        font = group.readEntry("toolBarFont", QFont());
    } else if (name == "system-menu") {
        font = group.readEntry("menuFont", QFont());
    } else if (name == "system-window") {
        font = group.readEntry("activeFont", QFont());
    } else {
        font = QFont(QString::fromUtf8(name));
    }

    return font;
}

void logError(QByteArrayView property, QByteArrayView value, const PropertyFunctions::Error &error, const LoadingContext &context)
{
    qCWarning(UNION_PLASMASVG) << "Failed setting value" << value << "of property" << property << "of rule" << context.selectors() << ":"
                               << qPrintable(error.message);
}

bool PlasmaSvgLoader::load(Theme::Ptr theme)
{
    QDir dir(u":/org/kde/union/input/plasmasvg"_s);
    const auto entries = dir.entryList({u"*.yml"_s});
    if (entries.isEmpty()) {
        qCWarning(UNION_PLASMASVG) << "No style definitions found";
        return false;
    }

    for (auto entry : entries) {
        QFile file(dir.absoluteFilePath(entry));
        if (!file.open(QFile::ReadOnly)) {
            qCWarning(UNION_PLASMASVG) << "Could not read file" << entry << file.errorString();
            continue;
        }

        auto data = file.readAll().toStdString();
        ryml::Tree parsed;
        try {
            parsed = ryml::parse_in_place(ryml::to_substr(data));
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

        LoadingContext context(theme);
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

        auto style = createStyle(child, context);
        if (style) {
            context.theme->insert(style);
        }
    }
}

SelectorList createSelectors(ryml::ConstNodeRef node)
{
    SelectorList selectors;

    with_child(node, "type", [&](auto node) {
        selectors.append(Selector::create<SelectorType::Type>(value<QString>(node)));
    });

    with_child(node, "id", [&](auto node) {
        selectors.append(Selector::create<SelectorType::Id>(value<QString>(node)));
    });

    with_child(node, "state", [&](auto node) {
        selectors.append(Selector::create<SelectorType::State>(NODE_Q_ENUM_VALUE(Element, State, node)));
        // Support value or list
        if (node.has_val()) {
            selectors.append(Selector::create<SelectorType::State>(NODE_Q_ENUM_VALUE(Element, State, node)));
        } else if (node.is_seq() && node.has_children()) {
            SelectorList allOfList;
            for (auto child : node.children()) {
                if (!child.has_val()) {
                    continue;
                }
                allOfList.append(Selector::create<SelectorType::State>(NODE_Q_ENUM_VALUE(Element, State, child)));
            }
            selectors.append(Selector::create<SelectorType::AllOf>(allOfList));
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
            selectors.append(Selector::create<SelectorType::AllOf>(allOfList));
        }
    });

    with_child(node, "attributes", [&](auto node) {
        if (node.has_children()) {
            SelectorList allOfList;
            for (auto child : node.children()) {
                if (!child.is_keyval()) {
                    continue;
                }
                // clang-format off
                allOfList.append(Selector::create<SelectorType::Attribute>(
                    std::make_pair<QString, QVariant>(QString::fromLatin1(child.key()),
                                                      QString::fromLatin1(child.val()))
                ));
                // clang-format on
            }
            selectors.append(Selector::create<SelectorType::AllOf>(allOfList));
        }
    });

    with_child(node, "colorSet", [&](auto node) {
        // Support value or list
        if (node.has_val()) {
            selectors.append(Selector::create<SelectorType::ColorSet>(NODE_Q_ENUM_VALUE(Element, ColorSet, node)));
        } else if (node.is_seq() && node.has_children()) {
            // You can't display multiple color sets at once,
            // but you could show the same appearance for multiple color sets.
            SelectorList anyOfList;
            for (auto child : node.children()) {
                if (!child.has_val()) {
                    continue;
                }
                anyOfList.append(Selector::create<SelectorType::ColorSet>(NODE_Q_ENUM_VALUE(Element, ColorSet, child)));
            }
            selectors.append(Selector::create<SelectorType::AnyOf>(anyOfList));
        }
    });

    return selectors;
}

StyleRule::Ptr PlasmaSvgLoader::createStyle(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto style = StyleRule::create();
    SelectorList selectors = createSelectors(node);
    SelectorList currentSelectors = context.selectors();
    if (selectors.size() == 1) {
        currentSelectors.append(selectors.first());
    } else {
        currentSelectors.append(Selector::create<SelectorType::AllOf>(selectors));
    }

    style->setSelectors(currentSelectors);

    auto cleanup = context.pushFromNode(node, selectors);

    // element->addAttribute(u"plugin"_qs, QString::fromUtf16(PluginName));
    // element->addAttribute(u"themeName"_qs, m_theme.themeName());

    StyleProperty properties;

    with_child(node, "layout", [&](auto node) {
        properties.setLayout(createLayoutProperty(node, context));
    });

    with_child(node, "text", [&](auto node) {
        properties.setText(createTextProperty(node, context));
    });

    with_child(node, "background", [&](auto node) {
        properties.setBackground(createBackgroundProperty(node, context));
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

    auto cleanup = context.pushFromNode(node);

    LayoutProperty layout;

    with_child(node, "width", [&](auto node) {
        auto value = PropertyFunctions::elementProperty<qreal>(node, context);
        if (value.has_value()) {
            layout.setWidth(value.value());
        } else {
            logError("layout", "width", value.error(), context);
        }
    });
    with_child(node, "height", [&](auto node) {
        auto value = PropertyFunctions::elementProperty<qreal>(node, context);
        if (value.has_value()) {
            layout.setHeight(value.value());
        } else {
            logError("layout", "height", value.error(), context);
        }
    });
    with_child(node, "spacing", [&](auto node) {
        auto value = PropertyFunctions::elementProperty<qreal>(node, context);
        if (value.has_value()) {
            layout.setSpacing(value.value());
        } else {
            logError("layout", "spacing", value.error(), context);
        }
    });
    with_child(node, "alignment", [&](auto node) {
        layout.setAlignment(value<Qt::Alignment>(node));
    });
    with_child(node, "padding", [&](auto node) {
        layout.setPadding(createSizeProperty(node, context));
    });
    with_child(node, "margins", [&](auto node) {
        layout.setMargins(createSizeProperty(node, context));
    });

    return layout;
}

std::optional<TextProperty> PlasmaSvgLoader::createTextProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    TextProperty text;
    with_child(node, "alignment", [&](auto node) {
        text.setAlignment(value<Qt::Alignment>(node));
    });
    with_child(node, "font", [&](auto node) {
        auto fontName = node.val();
        if (fontName.empty()) {
            return;
        }

        text.setFont(fontFromName(fontName));
    });

    return text;
}

std::optional<BackgroundProperty> PlasmaSvgLoader::createBackgroundProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    BackgroundProperty background;
    with_child(node, "color", [&](auto node) {
        auto value = PropertyFunctions::elementProperty<QColor>(node, context);
        if (value.has_value()) {
            background.setColor(value.value());
        } else {
            logError("background", "color", value.error(), context);
        }
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

std::optional<SizeProperty> PlasmaSvgLoader::createSizeProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    SizeProperty sizes;
    forEachEntry({"left"s, "right"s, "top"s, "bottom"s},
                 sizes,
                 {&SizeProperty::setLeft, &SizeProperty::setRight, &SizeProperty::setTop, &SizeProperty::setBottom},
                 node,
                 [&context](auto node) {
                     auto cleanup = context.pushFromNode(node);
                     auto value = PropertyFunctions::elementProperty<qreal>(node, context);
                     if (value.has_value()) {
                         return value.value();
                     } else {
                         logError("size", node.key(), value.error(), context);
                         return 0.0;
                     }
                 });

    return sizes;
}

std::optional<BorderProperty> PlasmaSvgLoader::createBorderProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
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
        return std::nullopt;
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
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    LineProperty line;
    with_child(node, "size", [&](auto node) {
        auto value = PropertyFunctions::elementProperty<qreal>(node, context);
        if (value.has_value()) {
            line.setSize(value.value());
        } else {
            logError("line", "size", value.error(), context);
        }
    });
    with_child(node, "image", [&](auto node) {
        line.setImage(createImageProperty(node, context));
    });
    return line;
}

std::optional<CornerProperty> PlasmaSvgLoader::createCornerProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    CornerProperty corner;
    with_child(node, "image", [&](auto node) {
        corner.setImage(createImageProperty(node, context));
    });
    with_child(node, "size", [&](auto node) {
        auto size = PropertyFunctions::elementProperty<QSizeF>(node, context);
        if (size.has_value()) {
            corner.setWidth(size.value().width());
            corner.setHeight(size.value().height());
        } else {
            logError("corner", "size", size.error(), context);
        }
    });
    return corner;
}

std::optional<ImageProperty> PlasmaSvgLoader::createImageProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    auto data = PropertyFunctions::elementProperty<QImage>(node, context);
    if (!data.has_value()) {
        logError("image", "data", data.error(), context);
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
        return std::nullopt;
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
