/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "PlasmaSvgLoader.h"

#include <QFile>
#include <QPainter>
#include <QStack>
#include <QSvgRenderer>

#include <Plasma/Theme>

#include <Definition.h>
#include <ElementIdentifier.h>
#include <Style.h>
#include <StyleElement.h>

#include "plasmasvg_logging.h"

using namespace Union;
using namespace Qt::StringLiterals;

constexpr char16_t PluginName[] = u"plasmasvg";

struct LoaderContext {
    YAML::Node node;
    StyleElement::Ptr element;
    std::shared_ptr<QSvgRenderer> renderer;
    QString prefix;
};

PlasmaSvgLoader::PlasmaSvgLoader(std::shared_ptr<Style> style, QObject *parent)
    : StyleLoader(style, u"plasmasvg"_qs, parent)
{
}

bool PlasmaSvgLoader::load()
{
    QFile file(u":/org/kde/union/input/plasmasvg/PlasmaSvgStructure.yml"_qs);
    if (!file.open(QFile::ReadOnly)) {
        return false;
    }

    YAML::Node parsed;
    try {
        parsed = YAML::Load(file.readAll().constData());
    } catch (const YAML::Exception &e) {
        qCWarning(UNION_PLASMASVG) << "Could not parse PlasmaSvgStructure.yml:" << e.what();
        return false;
    }

    for (auto itr = parsed.begin(); itr != parsed.end(); ++itr) {
        if (!itr->IsMap()) {
            qCWarning(UNION_PLASMASVG) << "Unexpected input in PlasmaSvgStructure.yml";
            continue;
        }
        QStack<LoaderContext> context;
        auto element = createElement(*itr, context);
        style()->insert(element);
    }

    return true;
}

StyleElement::Ptr PlasmaSvgLoader::createElement(const YAML::Node &node, QStack<LoaderContext> &context)
{
    QString type;
    if (node["type"]) {
        type = QString::fromStdString(node["type"].Scalar());
    } else if (!context.isEmpty()) {
        type = context.top().element->type();
    }

    LoaderContext currentContext;
    auto element = std::make_shared<StyleElement>(type);
    currentContext.element = element;

    if (node["id"]) {
        element->setId(QString::fromStdString(node["id"].Scalar()));
    }

    if (node["state"]) {
        element->setState(QString::fromStdString(node["state"].Scalar()));
    }

    element->addAttribute(u"plugin"_qs, QString::fromUtf16(PluginName));
    element->addAttribute(u"themeName"_qs, m_theme.themeName());

    qDebug() << element->type() << element->state() << element->attributes();

    auto createChildren = [this, node, &context, &currentContext, &element]() {
        context.push(currentContext);
        for (auto itr = node["children"].begin(); itr != node["children"].end(); ++itr) {
            auto child = createElement(*itr, context);
            // child->setParentElement(element);
            // element->addChild(child);
            style()->insert(child);
        }
    };

    if (node["render"] && !node["render"].as<bool>()) {
        createChildren();
        return element;
    }

    std::shared_ptr<QSvgRenderer> renderer = nullptr;
    if (node["path"]) {
        auto fileName = m_theme.imagePath(QString::fromStdString(node["path"].Scalar()));
        renderer = std::make_shared<QSvgRenderer>(fileName);
    } else {
        renderer = context.top().renderer;
    }

    currentContext.renderer = renderer;

    QString prefix;
    if (node["prefix"]) {
        prefix = context.top().prefix + u"-" + QString::fromStdString(node["prefix"].Scalar());
    } else {
        prefix = context.top().prefix;
    }

    auto parentElement = context.top().element;

    Union::AreaDefinition area;
    area.image = renderElement(renderer,
                               prefix + QStringLiteral("-center"),
                               parentElement->background().value_or(AreaDefinition{}).image.value_or(ImageDefinition{}));
    element->setBackground(area);

    Union::BorderDefinition border;
    border.top = createLineDefinition(renderer,
                                      prefix + QStringLiteral("-top"),
                                      Qt::Horizontal,
                                      parentElement->border().value_or(BorderDefinition{}).top.value_or(LineDefinition{}));
    border.bottom = createLineDefinition(renderer,
                                         prefix + QStringLiteral("-bottom"),
                                         Qt::Horizontal,
                                         parentElement->border().value_or(BorderDefinition{}).bottom.value_or(LineDefinition{}));
    border.left = createLineDefinition(renderer,
                                       prefix + QStringLiteral("-left"),
                                       Qt::Vertical,
                                       parentElement->border().value_or(BorderDefinition{}).left.value_or(LineDefinition{}));
    border.right = createLineDefinition(renderer,
                                        prefix + QStringLiteral("-right"),
                                        Qt::Vertical,
                                        parentElement->border().value_or(BorderDefinition{}).right.value_or(LineDefinition{}));
    element->setBorder(border);

    Union::CornersDefinition corners;
    corners.topLeft = createCornerDefinition(renderer,
                                             prefix + u"-topleft"_qs,
                                             parentElement->corners().value_or(CornersDefinition{}).topLeft.value_or(CornerDefinition{}));
    corners.topRight = createCornerDefinition(renderer,
                                              prefix + u"-topright"_qs,
                                              parentElement->corners().value_or(CornersDefinition{}).topRight.value_or(CornerDefinition{}));
    corners.bottomLeft = createCornerDefinition(renderer,
                                                prefix + u"-bottomleft"_qs,
                                                parentElement->corners().value_or(CornersDefinition{}).bottomLeft.value_or(CornerDefinition{}));
    corners.bottomRight = createCornerDefinition(renderer,
                                                 prefix + u"-bottomright"_qs,
                                                 parentElement->corners().value_or(CornersDefinition{}).bottomRight.value_or(CornerDefinition{}));
    element->setCorners(corners);

    Union::SizeDefinition padding;
    padding.left = Union::Size(elementSize(renderer, prefix + u"-hint-left-margin").width(), Union::Size::Unit::LogicalPixels);
    padding.right = Union::Size(elementSize(renderer, prefix + u"-hint-right-margin").width(), Union::Size::Unit::LogicalPixels);
    padding.top = Union::Size(elementSize(renderer, prefix + u"-hint-top-margin").height(), Union::Size::Unit::LogicalPixels);
    padding.bottom = Union::Size(elementSize(renderer, prefix + u"-hint-bottom-margin").height(), Union::Size::Unit::LogicalPixels);
    element->setPadding(padding);

    createChildren();
    return element;
}

std::optional<LineDefinition> PlasmaSvgLoader::createLineDefinition(std::shared_ptr<QSvgRenderer> renderer,
                                                                    const QString &elementName,
                                                                    Qt::Orientation orientation,
                                                                    LineDefinition parentLine)
{
    auto image = renderElement(renderer, elementName, parentLine.image.value_or(ImageDefinition{}));
    if (image) {
        LineDefinition definition;
        definition.image = image;

        if (orientation == Qt::Horizontal) {
            definition.size = definition.image->height;
        } else {
            definition.size = definition.image->width;
        }

        return definition;
    }

    return std::nullopt;
}

std::optional<CornerDefinition> PlasmaSvgLoader::createCornerDefinition(std::shared_ptr<QSvgRenderer> renderer,
                                                                        const QString &elementName,
                                                                        CornerDefinition parentCorner)
{
    auto image = renderElement(renderer, elementName, parentCorner.image.value_or(ImageDefinition{}));
    if (image) {
        CornerDefinition definition;
        definition.image = image;
        return definition;
    }

    return std::nullopt;
}

std::optional<ImageDefinition> PlasmaSvgLoader::renderElement(std::shared_ptr<QSvgRenderer> renderer,
                                                              const QString &elementName,
                                                              const ImageDefinition &parentImage)
{
    if (!renderer->elementExists(elementName)) {
        return std::nullopt;
    }

    auto size = elementSize(renderer, elementName);

    QImage image(size.toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);

    if (!parentImage.imageData.isNull()) {
        painter.drawImage(QRectF{0.0, 0.0, size.width(), size.height()}, parentImage.imageData);
    }

    renderer->render(&painter, elementName);

    Union::ImageDefinition result;
    result.imageData = image;
    result.width = Union::Size(image.width(), Union::Size::Unit::DevicePixels);
    result.height = Union::Size(image.height(), Union::Size::Unit::DevicePixels);

    return result;
}

QSizeF PlasmaSvgLoader::elementSize(std::shared_ptr<QSvgRenderer> renderer, const QString &elementName)
{
    if (!renderer->elementExists(elementName)) {
        return QSize{};
    }

    return renderer->transformForElement(elementName).map(renderer->boundsOnElement(elementName)).boundingRect().size();
}
