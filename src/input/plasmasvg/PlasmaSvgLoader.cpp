/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "PlasmaSvgLoader.h"

#include <unordered_set>

#include <QDir>
#include <QFile>
#include <QPainter>
#include <QStack>
#include <QSvgRenderer>

#include <Plasma/Theme>

#include <Definition.h>
#include <Style.h>
#include <Theme.h>

#include "plasmasvg_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;
using namespace std::string_literals;

constexpr char16_t PluginName[] = u"plasmasvg";

QString nodeToString(ryml::ConstNodeRef node)
{
    if (!node.is_val() && !node.is_keyval()) {
        return QString{};
    }

    std::string tmp;
    node >> tmp;
    return QString::fromStdString(tmp);
}

template<typename I, typename O, typename F>
void forEachEntry(const std::initializer_list<I> &input, const std::initializer_list<O *> output, ryml::ConstNodeRef &node, F callback)
{
    Q_ASSERT(input.size() == output.size());

    auto inputItr = input.begin();
    auto outputItr = output.begin();
    for (; inputItr != input.end() && outputItr != output.end(); ++inputItr, ++outputItr) {
        *(*outputItr) = callback(node[ryml::to_csubstr(*inputItr)]);
    }
}

struct ContextData {
    QStack<SelectorList> selectors;
    QStack<QString> paths;
    QStack<QString> prefixes;
    QStack<QString> elementNames;
};

struct ContextCleanup {
    ContextCleanup(ContextData &_data)
        : data(_data)
    {
    }

    ~ContextCleanup()
    {
        if (popSelectors) {
            data.selectors.pop();
        }

        if (popPaths) {
            data.paths.pop();
        }

        if (popPrefixes) {
            data.prefixes.pop();
        }

        if (popElementNames) {
            data.elementNames.pop();
        }
    }

    ContextData &data;
    bool popSelectors : 1 = false;
    bool popPaths : 1 = false;
    bool popPrefixes : 1 = false;
    bool popElementNames : 1 = false;
};

struct LoadingContext {
    ContextData data;
    QStack<SelectorList> &selectors = data.selectors;
    QStack<QString> &paths = data.paths;
    QStack<QString> &prefixes = data.prefixes;
    QStack<QString> &elementNames = data.elementNames;

    ContextCleanup pushFromNode(ryml::ConstNodeRef node)
    {
        ContextCleanup cleanup(data);

        if (!node.is_map()) {
            return cleanup;
        }

        if (node.has_child("path")) {
            data.paths.push(nodeToString(node["path"]));
            cleanup.popPaths = true;
        }

        if (node.has_child("prefix")) {
            data.prefixes.push(nodeToString(node["prefix"]));
            cleanup.popPrefixes = true;
        }

        if (node.has_child("element")) {
            data.elementNames.push(nodeToString(node["element"]));
            cleanup.popElementNames = true;
        }

        return cleanup;
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

    if (node.has_child("type")) {
        selectors.append(Selector(Selector::SelectorType::Type, nodeToString(node["type"])));
    }

    if (node.has_child("id")) {
        selectors.append(Selector(Selector::SelectorType::Id, nodeToString(node["id"])));
    }

    if (node.has_child("state")) {
        selectors.append(Selector(Selector::SelectorType::State, nodeToString(node["state"])));
    }

    SelectorList currentSelectors = context.selectors.isEmpty() ? SelectorList() : context.selectors.top();
    if (selectors.size() == 1) {
        currentSelectors.append(selectors.first());
    } else {
        currentSelectors.append(Selector(Selector::SelectorType::AllOf, QVariant::fromValue(selectors)));
    }

    style->setSelectors(currentSelectors);
    context.selectors.push(currentSelectors);

    auto cleanup = context.pushFromNode(node);
    cleanup.popSelectors = true;

    // element->addAttribute(u"plugin"_qs, QString::fromUtf16(PluginName));
    // element->addAttribute(u"themeName"_qs, m_theme.themeName());

    if (node.has_child("padding")) {
        style->setPadding(createSizeDefinition(node["padding"], context));
    }

    if (node.has_child("border")) {
        style->setBorder(createBorderDefinition(node["border"], context));
    }

    if (node.has_child("corners")) {
        style->setCorners(createCornersDefinition(node["corners"], context));
    }

    if (node.has_child("background")) {
        style->setBackground(createAreaDefinition(node["background"], context));
    }

    if (node.has_child("children")) {
        createStyles(node["children"], context);
    }

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
    area.size = elementProperty(node["size"], context).toSizeF();
    area.image = createImageDefinition(node["image"], context);
    return area;
}

std::optional<Union::LineDefinition> PlasmaSvgLoader::createLineDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::LineDefinition line;
    line.size = elementProperty(node["size"], context).toReal();
    line.image = createImageDefinition(node["image"], context);
    return line;
}

std::optional<Union::CornerDefinition> PlasmaSvgLoader::createCornerDefinition(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    auto cleanup = context.pushFromNode(node);

    Union::CornerDefinition corner;
    corner.image = createImageDefinition(node["image"], context);
    auto size = elementProperty(node["size"], context).toSize();
    corner.width = size.width();
    corner.height = size.height();
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

QVariant PlasmaSvgLoader::elementProperty(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (node.is_val() || node.is_keyval()) {
        return constantValue(node, context);
    }

    if (!node.is_map()) {
        return QVariant{};
    }

    auto name = node["property"].val();
    if (name.empty()) {
        return QVariant{};
    }

    auto cleanup = context.pushFromNode(node);

    if (name == "constant") {
        return constantValue(node, context);
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

QVariant PlasmaSvgLoader::constantValue(ryml::ConstNodeRef node, LoadingContext &)
{
    auto value = node;
    if (node.is_map() && node.has_child("value")) {
        value = node["value"];
    }

    qreal result;
    value >> result;

    return result;
}

std::optional<QSizeF> PlasmaSvgLoader::elementSize(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto renderer = rendererForPath(context.paths.top());
    if (!renderer) {
        return std::nullopt;
    }

    const auto element = context.prefixedElementName();
    if (!renderer->elementExists(element)) {
        qCDebug(UNION_PLASMASVG) << "Could not find element" << element;
        return std::nullopt;
    }

    auto size = renderer->transformForElement(element).map(renderer->boundsOnElement(element)).boundingRect().size();
    return size;
}

QImage PlasmaSvgLoader::elementImage(ryml::ConstNodeRef node, LoadingContext &context)
{
    Q_UNUSED(node)

    auto renderer = rendererForPath(context.paths.top());
    if (!renderer) {
        return QImage{};
    }

    const auto element = context.prefixedElementName();
    if (!renderer->elementExists(element)) {
        qCDebug(UNION_PLASMASVG) << "Could not find element" << element;
        return QImage{};
    }

    auto size = renderer->transformForElement(element).map(renderer->boundsOnElement(element)).boundingRect().size();

    QImage image(size.toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    renderer->render(&painter, element);

    return image;
}

QImage PlasmaSvgLoader::elementImageBlend(ryml::ConstNodeRef node, LoadingContext &context)
{
    if (!node.has_child("elements")) {
        return QImage{};
    }

    QList<QImage> images;
    int maxWidth = 0;
    int maxHeight = 0;
    for (auto child : node["elements"].children()) {
        context.elementNames.push(nodeToString(child));
        auto image = elementImage(node, context);
        context.elementNames.pop();

        images.append(image);
        maxWidth = std::max(maxWidth, image.width());
        maxHeight = std::max(maxHeight, image.height());
    }

    QImage result(maxWidth, maxHeight, QImage::Format_ARGB32);
    result.fill(Qt::transparent);

    auto itr = context.prefixes.rbegin();
    auto prefix = *itr;
    while (prefix.isEmpty()) {
        itr++;
        prefix = *itr;
    }

    QPainter painter(&result);
    for (const auto &image : images) {
        painter.drawImage(geometry, image);
    }

    return result;
}

std::shared_ptr<QSvgRenderer> PlasmaSvgLoader::rendererForPath(QAnyStringView path)
{
    auto pathString = path.toString();
    if (m_renderers.contains(pathString)) {
        return m_renderers.value(pathString);
    }

    const auto fileName = m_theme.imagePath(pathString);
    if (fileName.isEmpty()) {
        return nullptr;
    }

    auto renderer = std::make_shared<QSvgRenderer>(fileName);
    m_renderers.insert(pathString, renderer);
    return renderer;
}
