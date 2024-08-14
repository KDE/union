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

#include <Definition.h>
#include <StyleRule.h>
#include <Theme.h>

#include "LoadingContext.h"
#include "PlasmaSvgRenderer.h"
#include "PropertyFunctions.h"

#include "plasmasvg_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;
using namespace std::string_literals;

constexpr char16_t PluginName[] = u"plasmasvg";

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

    with_child(node, "type", [&](auto node){
        selectors.append(Selector::create<SelectorType::Type>(value<QString>(node)));
    });

    with_child(node, "id", [&](auto node){
        selectors.append(Selector::create<SelectorType::Id>(value<QString>(node)));
    });

    with_child(node, "state", [&](auto node){
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

    with_child(node, "hints", [&](auto node){
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

    with_child(node, "attributes", [&](auto node){
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

    with_child(node, "colorSet", [&](auto node){
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

    with_child(node, "margins", [&](auto node){
        style->setMargins(createSizeDefinition(node, context));
    });

    with_child(node, "padding", [&](auto node){
        style->setPadding(createSizeDefinition(node, context));
    });

    with_child(node, "border", [&](auto node){
        style->setBorder(createBorderDefinition(node, context));
    });

    with_child(node, "corners", [&](auto node){
        style->setCorners(createCornersDefinition(node, context));
    });

    with_child(node, "background", [&](auto node){
        style->setBackground(createAreaDefinition(node, context));
    });

    with_child(node, "shadow", [&](auto node){
        style->setShadow(createShadowDefinition(node, context));
    });

    with_child(node, "text", [&](auto node){
        style->setText(createTextDefinition(node, context));
    });

    with_child(node, "icon", [&](auto node){
        style->setIcon(createIconDefinition(node, context));
    });

    with_child(node, "children", [&](auto node){
        createStyles(node, context);
    });

    return style;
}

std::optional<Union::SizeDefinition> PlasmaSvgLoader::createSizeDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::SizeDefinition sizes;
    forEachEntry({"left"s, "right"s, "top"s, "bottom"s},
                 {&sizes.left, &sizes.right, &sizes.top, &sizes.bottom},
                 node,
                 [this, &context](ryml::ConstNodeRef node) {
                     auto cleanup = context.pushFromNode(node);
                     return elementProperty(node, context).toReal();
                 });

    return sizes;
}

std::optional<Union::BorderDefinition> PlasmaSvgLoader::createBorderDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::BorderDefinition border;
    forEachEntry({"left"s, "right"s, "top"s, "bottom"s},
                 {&border.left, &border.right, &border.top, &border.bottom},
                 node,
                 [this, &context](ryml::ConstNodeRef node) {
                     auto cleanup = context.pushFromNode(node);
                     return createLineDefinition(node, context);
                 });

    return border;
}

std::optional<Union::CornersDefinition> PlasmaSvgLoader::createCornersDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::CornersDefinition corners;
    forEachEntry({"top-left"s, "top-right"s, "bottom-left"s, "bottom-right"s},
                 {&corners.topLeft, &corners.topRight, &corners.bottomLeft, &corners.bottomRight},
                 node,
                 [this, &context](ryml::ConstNodeRef node) {
                     auto cleanup = context.pushFromNode(node);
                     return createCornerDefinition(node, context);
                 });

    return corners;
}

std::optional<Union::AreaDefinition> PlasmaSvgLoader::createAreaDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::AreaDefinition area;
    with_child(node, "size", [&](auto node){
        area.size = elementProperty(node, context).toSizeF();
    });
    with_child(node, "image", [&](auto node){
        area.image = createImageDefinition(node, context);
    });
    return area;
}

std::optional<Union::LineDefinition> PlasmaSvgLoader::createLineDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::LineDefinition line;
    with_child(node, "size", [&](auto node){
        line.size = elementProperty(node, context).toReal();
    });
    with_child(node, "image", [&](auto node){
        line.image = createImageDefinition(node, context);
    });
    return line;
}

std::optional<Union::CornerDefinition> PlasmaSvgLoader::createCornerDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::CornerDefinition corner;
    with_child(node, "image", [&](auto node){
        corner.image = createImageDefinition(node, context);
    });
    with_child(node, "size", [&](auto node){
        auto size = elementProperty(node, context).toSizeF();
        corner.width = size.width();
        corner.height = size.height();
    });
    return corner;
}

std::optional<Union::ImageDefinition> PlasmaSvgLoader::createImageDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::ImageDefinition image;
    image.imageData = elementProperty(node, context).value<QImage>();
    image.width = image.imageData.width();
    image.height = image.imageData.height();
    image.flags = Union::RepeatBoth;
    return image;
}

std::optional<Union::ShadowDefinition> PlasmaSvgLoader::createShadowDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::ShadowDefinition shadow;
    with_child(node, "offsets", [&](auto node){
        shadow.offsets = createSizeDefinition(node, context);
    });

    forEachEntry({"left", "right", "top", "bottom"}, {&shadow.left, &shadow.right, &shadow.top, &shadow.bottom}, node, [this, &context](auto node) {
        context.pushFromNode(node);
        return createLineDefinition(node, context);
    });

    forEachEntry({"top-left", "top-right", "bottom-left", "bottom-right"},
                 {&shadow.topLeft, &shadow.topRight, &shadow.bottomLeft, &shadow.bottomRight},
                 node,
                 [this, &context](auto node) {
                     context.pushFromNode(node);
                     return createCornerDefinition(node, context);
                 });

    return shadow;
}

std::optional<Union::TextDefinition> PlasmaSvgLoader::createTextDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::TextDefinition text;
    with_child(node, "align", [&](auto node){
        text.alignment = value<Qt::Alignment>(node);
    });
    with_child(node, "font", [&](auto node){
        auto fontName = node.val();
        if (fontName.empty()) {
            return;
        }

        auto config = KSharedConfig::openConfig(u"kdeglobals"_s);
        auto group = config->group(u"General"_s);

        if (fontName == "system-normal") {
            text.font = group.readEntry("font", QFont());
        } else if (fontName == "system-fixed") {
            text.font = group.readEntry("fixed", QFont());
        } else if (fontName == "system-small") {
            text.font = group.readEntry("smallestReadableFont", QFont());
        } else if (fontName == "system-toolbar") {
            text.font = group.readEntry("toolBarFont", QFont());
        } else if (fontName == "system-menu") {
            text.font = group.readEntry("menuFont", QFont());
        } else if (fontName == "system-window") {
            text.font = group.readEntry("activeFont", QFont());
        }
    });

    return text;
}

std::optional<Union::IconDefinition> PlasmaSvgLoader::createIconDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map() || !node.has_children()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::IconDefinition icon;
    with_child(node, "color", [&](auto node){
        icon.color = value<QColor>(node);
    });
    auto valueForIconSizeName = [](c4::csubstr val) -> qreal {
        if (val == "small") {
            return 16;
        } else if (val == "small-medium") {
            return 22;
        } else if (val == "medium") {
            return 32;
        } else if (val == "large") {
            return 48;
        } else if (val == "huge") {
            return 64;
        } else if (val == "enormous") {
            return 128;
        }
        return 0;
    };
    with_child(node, "size", [&](auto node){
        if (!node.has_val()) {
            return;
        }
        auto val = node.val();
        if (val.empty() || ryml::read(node, &icon.size.rwidth())) {
            icon.size.setHeight(icon.size.width());
            return;
        }
        auto size = valueForIconSizeName(val);
        icon.size = {size, size};
    });
    auto setIconSource = [](auto &icon, auto node) {
        auto val = node.val();
        if (val.empty()) {
            return;
        }
        // Freedesktop icon names shouldn't contain spaces, colons, slashes or
        // backslashes. If the string has those, it's a path or url.
        // https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html#guidelines
        if (node.val().first_of(" :/\\") == c4::csubstr::npos) {
            icon.name = value<QString>(node);
            return;
        }
        icon.url = value<QUrl>(node);
    };
    with_child(node, "source", [&](auto node){
        if (node.has_val()) {
            setIconSource(icon, node);
        } else if (node.is_seq() && node.has_children()) {
            for (auto child : node.children()) {
                if (!child.has_val()) {
                    continue;
                }
                setIconSource(icon, child);
                if (!icon.name.isEmpty() && !QIcon::hasThemeIcon(icon.name)) {
                    icon.name = QString{};
                    continue;
                }
                if (!icon.url.isEmpty() && (!icon.url.isValid()
                    || !icon.url.isLocalFile()
                    || QIcon::fromTheme(icon.url.toLocalFile()).isNull())) {
                    icon.url = QUrl{};
                    continue;
                }
                break;
            }
        }
    });
    return icon;
}
