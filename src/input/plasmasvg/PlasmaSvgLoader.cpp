/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "PlasmaSvgLoader.h"

#include <unordered_set>

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

        SelectorList selectors;
        createStyles(root, selectors);
    }

    m_renderers.clear();

    return true;
}

void PlasmaSvgLoader::createStyles(ryml::ConstNodeRef node, const SelectorList &parentSelectors)
{
    for (auto child : node.children()) {
        if (!child.is_map()) {
            continue;
        }

        auto style = createStyle(child, parentSelectors);
        if (style) {
            theme()->insert(style);
        }
    }
}

Style::Ptr PlasmaSvgLoader::createStyle(ryml::ConstNodeRef node, const SelectorList &parentSelectors)
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

    QList<Selector> currentSelectors = parentSelectors;
    if (selectors.size() == 1) {
        currentSelectors.append(selectors.first());
    } else {
        currentSelectors.append(Selector(Selector::SelectorType::AllOf, QVariant::fromValue(selectors)));
    }

    style->setSelectors(currentSelectors);

    // element->addAttribute(u"plugin"_qs, QString::fromUtf16(PluginName));
    // element->addAttribute(u"themeName"_qs, m_theme.themeName());

    if (node.has_child("padding")) {
        style->setPadding(createSizeDefinition(node["padding"]));
    }

    if (node.has_child("border")) {
        style->setBorder(createBorderDefinition(node["border"]));
    }

    if (node.has_child("corners")) {
        style->setCorners(createCornersDefinition(node["corners"]));
    }

    if (node.has_child("background")) {
        style->setBackground(createAreaDefinition(node["background"]));
    }

    if (node.has_child("children")) {
        createStyles(node["children"], currentSelectors);
    }

    return style;
}

std::optional<Union::SizeDefinition> PlasmaSvgLoader::createSizeDefinition(ryml::ConstNodeRef node)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    Union::SizeDefinition sizes;
    forEachEntry({"left"s, "right"s, "top"s, "bottom"s}, {&sizes.left, &sizes.right, &sizes.top, &sizes.bottom}, node, [this](ryml::ConstNodeRef node) {
        return elementProperty(node).toReal();
    });

    return sizes;
}

std::optional<Union::BorderDefinition> PlasmaSvgLoader::createBorderDefinition(ryml::ConstNodeRef node)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    Union::BorderDefinition border;
    forEachEntry({"left"s, "right"s, "top"s, "bottom"s}, {&border.left, &border.right, &border.top, &border.bottom}, node, [this](ryml::ConstNodeRef node) {
        return createLineDefinition(node);
    });

    return border;
}

std::optional<Union::CornersDefinition> PlasmaSvgLoader::createCornersDefinition(ryml::ConstNodeRef node)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    Union::CornersDefinition corners;
    forEachEntry({"top-left"s, "top-right"s, "bottom-left"s, "bottom-right"s},
                 {&corners.topLeft, &corners.topRight, &corners.bottomLeft, &corners.bottomRight},
                 node,
                 [this](ryml::ConstNodeRef node) {
                     return createCornerDefinition(node);
                 });

    return corners;
}

std::optional<Union::AreaDefinition> PlasmaSvgLoader::createAreaDefinition(ryml::ConstNodeRef node)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    Union::AreaDefinition area;
    area.size = elementProperty(node["size"]).toSizeF();
    area.image = createImageDefinition(node["image"]);
    return area;
}

std::optional<Union::LineDefinition> PlasmaSvgLoader::createLineDefinition(ryml::ConstNodeRef node)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    Union::LineDefinition line;
    line.size = elementProperty(node["size"]).toReal();
    line.image = createImageDefinition(node["image"]);
    return line;
}

std::optional<Union::CornerDefinition> PlasmaSvgLoader::createCornerDefinition(ryml::ConstNodeRef node)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    Union::CornerDefinition corner;
    corner.image = createImageDefinition(node["image"]);
    auto size = elementProperty(node["size"]).toSize();
    corner.width = size.width();
    corner.height = size.height();
    return corner;
}

std::optional<Union::ImageDefinition> PlasmaSvgLoader::createImageDefinition(ryml::ConstNodeRef node)
{
    if (!node.is_map()) {
        return std::nullopt;
    }

    Union::ImageDefinition image;
    image.imageData = elementProperty(node).value<QImage>();
    image.width = image.imageData.width();
    image.height = image.imageData.height();
    image.flags = Union::RepeatBoth;
    return image;
}

QVariant PlasmaSvgLoader::elementProperty(ryml::ConstNodeRef node)
{
    if (!node.is_map()) {
        return QVariant{};
    }

    auto name = node["property"].val();
    if (name.empty()) {
        return QVariant{};
    }

    if (name == "element-size") {
        return elementSize(nodeToString(node["path"]), nodeToString(node["element"]));
    }

    if (name == "element-width") {
        return elementWidth(nodeToString(node["path"]), nodeToString(node["element"]));
    }

    if (name == "element-height") {
        return elementHeight(nodeToString(node["path"]), nodeToString(node["element"]));
    }

    if (name == "element-image") {
        return elementImage(nodeToString(node["path"]), nodeToString(node["element"]));
    }

    if (name == "element-image-blend") {
        return elementImageBlend(nodeToString(node["path"]), node["elements"]);
    }

    return QVariant{};
}

QSizeF PlasmaSvgLoader::elementSize(QAnyStringView path, QAnyStringView element)
{
    auto renderer = rendererForPath(path);
    if (!renderer) {
        return QSizeF{};
    }

    const auto elementString = element.toString();
    if (!renderer->elementExists(elementString)) {
        return QSizeF{};
    }

    return renderer->transformForElement(elementString).map(renderer->boundsOnElement(elementString)).boundingRect().size();
}

qreal PlasmaSvgLoader::elementWidth(QAnyStringView path, QAnyStringView element)
{
    const auto size = elementSize(path, element);
    if (size.isValid()) {
        return size.width();
    }
    return 0.0;
}

qreal PlasmaSvgLoader::elementHeight(QAnyStringView path, QAnyStringView element)
{
    const auto size = elementSize(path, element);
    if (size.isValid()) {
        return size.height();
    }
    return 0.0;
}

QImage PlasmaSvgLoader::elementImage(QAnyStringView path, QAnyStringView element)
{
    auto renderer = rendererForPath(path);
    if (!renderer) {
        return QImage{};
    }

    const auto elementString = element.toString();
    if (!renderer->elementExists(elementString)) {
        return QImage{};
    }

    auto size = elementSize(path, element);

    QImage image(size.toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    renderer->render(&painter, elementString);

    return image;
}

QImage PlasmaSvgLoader::elementImageBlend(QAnyStringView path, ryml::ConstNodeRef elements)
{
    if (!elements.is_seq()) {
        return QImage{};
    }

    QList<QImage> images;
    int maxWidth = 0;
    int maxHeight = 0;
    for (auto child : elements.children()) {
        auto image = elementImage(path, nodeToString(child));
        images.append(image);
        maxWidth = std::max(maxWidth, image.width());
        maxHeight = std::max(maxHeight, image.height());
    }

    QRect geometry(0, 0, maxWidth, maxHeight);
    QImage result(maxWidth, maxHeight, QImage::Format_ARGB32);
    result.fill(Qt::transparent);

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
