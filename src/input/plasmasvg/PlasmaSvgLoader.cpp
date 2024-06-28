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
#include <QPainter>
#include <QStack>

#include <KConfigGroup>
#include <Plasma/Theme>

#include <Definition.h>
#include <Style.h>
#include <Theme.h>

#include "PlasmaSvgRenderer.h"

#include "plasmasvg_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;
using namespace std::string_literals;

constexpr char16_t PluginName[] = u"plasmasvg";

std::size_t qHash(const RendererId &renderer, std::size_t seed)
{
    return qHashMulti(seed, renderer.path, renderer.colorGroup, renderer.colorSet);
}

struct ContextData {
    QStack<SelectorList> selectors;
    QStack<QString> paths;
    QStack<QString> prefixes;
    QStack<QString> elementNames;
    QStack<Element::ColorSet> colorSets;
};

struct ContextCleanup {
    enum class CleanupFlag {
        Selector = 1 << 0,
        Path = 1 << 1,
        Prefix = 1 << 2,
        ElementName = 1 << 3,
        ColorSet = 1 << 4,
    };
    Q_DECLARE_FLAGS(CleanupFlags, CleanupFlag)

    ContextCleanup(ContextData &_data)
        : data(_data)
    {
    }

    ~ContextCleanup()
    {
        if (flags & CleanupFlag::Selector && !data.selectors.isEmpty()) {
            data.selectors.pop();
        }

        if (flags & CleanupFlag::Path && !data.paths.isEmpty()) {
            data.paths.pop();
        }

        if (flags & CleanupFlag::Prefix && !data.prefixes.isEmpty()) {
            data.prefixes.pop();
        }

        if (flags & CleanupFlag::ElementName && !data.elementNames.isEmpty()) {
            data.elementNames.pop();
        }

        if (flags & CleanupFlag::ColorSet && !data.colorSets.isEmpty()) {
            data.colorSets.pop();
        }
    }

    ContextData &data;
    CleanupFlags flags;
};

struct LoadingContext {
    ContextData data;

    ContextCleanup pushFromNode(ryml::ConstNodeRef node, const SelectorList &selectorsToPush = SelectorList{})
    {
        ContextCleanup cleanup(data);

        if (!selectorsToPush.isEmpty()) {
            data.selectors.push(selectorsToPush);
            cleanup.flags |= ContextCleanup::CleanupFlag::Selector;
        }

        if (!node.is_map()) {
            return cleanup;
        }

        with_child(node, "path", [&](auto node){
            data.paths.push(value<QString>(node));
            cleanup.flags |= ContextCleanup::CleanupFlag::Path;
        });

        with_child(node, "prefix", [&](auto node){
            data.prefixes.push(value<QString>(node));
            cleanup.flags |= ContextCleanup::CleanupFlag::Prefix;
        });

        with_child(node, "element", [&](auto node){
            data.elementNames.push(value<QString>(node));
            cleanup.flags |= ContextCleanup::CleanupFlag::ElementName;
        });

        with_child(node, "colorSet", [&](auto node){
            data.colorSets.push(NODE_Q_ENUM_VALUE(Element, ColorSet, node));
        });

        return cleanup;
    }

    SelectorList selectors()
    {
        return data.selectors.isEmpty() ? SelectorList() : data.selectors.top();
    }

    QString prefixedElementName()
    {
        if (data.elementNames.isEmpty()) {
            return QString{};
        }

        auto element = data.elementNames.top();
        if (!data.prefixes.isEmpty()) {
            auto prefix = data.prefixes.top();
            if (!prefix.isEmpty()) {
                element = prefix + u"-" + element;
            }
        }

        return element;
    }

    QString path() const
    {
        return data.paths.isEmpty() ? QString{} : data.paths.top();
    }

    Element::ColorSet colorSet() const
    {
        return data.colorSets.isEmpty() ? Element::ColorSet::None : data.colorSets.top();
    }
};

PlasmaSvgLoader::PlasmaSvgLoader(std::shared_ptr<Theme> theme, QObject *parent)
    : StyleLoader(theme, u"plasmasvg"_qs, parent)
{
}

bool PlasmaSvgLoader::load()
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

        LoadingContext context;
        createStyles(root, context);
    }

    m_renderers.clear();

    return true;
}

void PlasmaSvgLoader::createStyles(ryml::ConstNodeRef node, LoadingContext &context)
{
    for (auto child : node.children()) {
        if (!child.is_map()) {
            continue;
        }

        auto style = createStyle(child, context);
        if (style) {
            theme()->insert(style);
        }
    }
}

Style::Ptr PlasmaSvgLoader::createStyle(ryml::ConstNodeRef node, LoadingContext &context)
{
    SelectorList selectors;
    auto style = Style::create();

    with_child(node, "type", [&](auto node){
        selectors.append(Selector::create<SelectorType::Type>(value<QString>(node)));
    });

    with_child(node, "id", [&](auto node){
        selectors.append(Selector::create<SelectorType::Id>(value<QString>(node)));
    });

    with_child(node, "state", [&](auto node){
        selectors.append(Selector::create<SelectorType::State>(NODE_Q_ENUM_VALUE(Element, State, node)));
    });

    with_child(node, "colorSet", [&](auto node){
        selectors.append(Selector::create<SelectorType::ColorSet>(NODE_Q_ENUM_VALUE(Element, ColorSet, node)));
    });

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

QVariant PlasmaSvgLoader::elementProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (node.has_val()) {
        return value<qreal>(node);
    }

    auto propertyNode = node.find_child("property");
    if (!propertyNode.valid()) {
        return QVariant{};
    }

    auto name = propertyNode.val();
    if (name.empty()) {
        return QVariant{};
    }

    auto cleanup = context.pushFromNode(node);

    if (name == "constant") {
        auto valueNode =  node.find_child("value");
        return valueNode.valid() ? value<qreal>(valueNode) : QVariant{};
    }

    if (name == "element-size") {
        return elementSize(node, context).value_or(QSizeF{});
    }

    if (name == "element-width") {
        auto size = elementSize(node, context);
        return size.has_value() ? size->width() : 0.0;
    }

    if (name == "element-height") {
        auto size = elementSize(node, context);
        return size.has_value() ? size->height() : 0.0;
    }

    if (name == "element-image") {
        return elementImage(node, context);
    }

    if (name == "element-image-blend") {
        return elementImageBlend(node, context);
    }

    return QVariant{};
}

std::optional<QSizeF> PlasmaSvgLoader::elementSize(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto renderer = rendererForPath(context.path(), QPalette::Normal, context.colorSet());
    if (!renderer) {
        return std::nullopt;
    }

    const auto element = context.prefixedElementName();
    if (!renderer->elementExists(element)) {
        qCDebug(UNION_PLASMASVG) << "Could not find element" << element;
        return std::nullopt;
    }

    auto size = renderer->elementRect(element).size();
    with_child(node, "invert", [&](auto node){
        if (value<bool>(node)) {
            size = QSizeF(-size.width(), -size.height());
        }
    });

    return size;
}

QImage PlasmaSvgLoader::elementImage(ryml::ConstNodeRef node, LoadingContext &context)
{
    Q_UNUSED(node)

    auto renderer = rendererForPath(context.path(), QPalette::Normal, context.colorSet());
    if (!renderer) {
        return QImage{};
    }

    const auto element = context.prefixedElementName();
    if (!renderer->elementExists(element)) {
        qCDebug(UNION_PLASMASVG) << "Could not find element" << element;
        return QImage{};
    }

    auto size = renderer->elementRect(element).size();

    QImage image(size.toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    renderer->render(&painter, element);

    return image;
}

QImage PlasmaSvgLoader::elementImageBlend(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto elementsNode = node.find_child("elements");
    if (!elementsNode.valid()) {
        return QImage{};
    }

    QList<QImage> images;
    int maxWidth = 0;
    int maxHeight = 0;
    for (auto child : elementsNode.children()) {
        context.data.elementNames.push(value<QString>(child));
        auto image = elementImage(node, context);
        context.data.elementNames.pop();

        images.append(image);
        maxWidth = std::max(maxWidth, image.width());
        maxHeight = std::max(maxHeight, image.height());
    }

    Qt::Alignment align;
    with_child(node, "align", [&](auto node){
        align = value<Qt::Alignment>(node);
    });

    QImage result(maxWidth, maxHeight, QImage::Format_ARGB32);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    for (const auto &image : images) {
        QPoint position;
        if (align & Qt::AlignLeft) {
            position.setX(0);
        } else if (align & Qt::AlignHCenter) {
            position.setX((maxWidth - image.width()) / 2);
        } else if (align & Qt::AlignRight) {
            position.setX(maxWidth - image.width());
        }

        if (align & Qt::AlignTop) {
            position.setY(0);
        } else if (align & Qt::AlignVCenter) {
            position.setY((maxHeight - image.height()) / 2);
        } else if (align & Qt::AlignBottom) {
            position.setY(maxHeight - image.height());
        }

        painter.drawImage(position, image);
    }

    return result;
}

std::shared_ptr<PlasmaSvgRenderer> PlasmaSvgLoader::rendererForPath(QAnyStringView path, QPalette::ColorGroup colorGroup, Element::ColorSet colorSet)
{
    auto pathString = path.toString();

    RendererId id{pathString, colorGroup, colorSet};

    if (m_renderers.contains(id)) {
        return m_renderers.value(id);
    }

    const auto fileName = m_theme.imagePath(pathString);
    if (fileName.isEmpty()) {
        return nullptr;
    }

    auto renderer = std::make_shared<PlasmaSvgRenderer>();
    renderer->setPath(fileName);
    renderer->setColorGroup(colorGroup);
    renderer->setColorSet(static_cast<KColorScheme::ColorSet>(static_cast<int>(colorSet) - 1));

    if (!renderer->load()) {
        return nullptr;
    }

    m_renderers.insert(id, renderer);
    return renderer;
}
