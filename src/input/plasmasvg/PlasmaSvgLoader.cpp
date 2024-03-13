/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "PlasmaSvgLoader.h"

#include <QPainter>
#include <QSvgRenderer>

#include <KSvg/ImageSet>

#include <Definition.h>
#include <ElementIdentifier.h>
#include <Style.h>
#include <StyleElement.h>

PlasmaSvgLoader::PlasmaSvgLoader(std::shared_ptr<Union::Style> style, QObject *parent)
    : Union::StyleLoader(style, u"plasmasvg"_qs, parent)
{
}

// bool PlasmaSvgLoader::load(const QUrl& url)
// {
//     Q_UNUSED(url);
//
//     Plasma::Theme theme;
//     auto normalButton = createElement(theme, QStringLiteral("widgets/button"), QStringLiteral("normal"));
//
//     Union::ElementIdentifier id;
//     id.setStyle(QStringLiteral("plasma-svg"));
//     id.setElement(QStringLiteral("button"));
//     id.setSubElement(QStringLiteral("normal"));
//     style()->insert(id, normalButton);
//
//     return true;
// }

bool PlasmaSvgLoader::loadElement(const Union::ElementIdentifier &element)
{
    KSvg::ImageSet imageSet;
    auto e = createElement(imageSet, u"widget"_qs + element.element(), element.subElement());
    style()->insert(element, e);
    return true;
}

std::shared_ptr<Union::StyleElement> PlasmaSvgLoader::createElement(KSvg::ImageSet &imageSet, const QString &elementName, const QString &prefix)
{
    auto element = std::make_shared<Union::StyleElement>();

    auto fileName = imageSet.imagePath(elementName);
    QSvgRenderer renderer(fileName);

    Union::AreaDefinition area;
    area.image = renderElement(renderer, prefix + QStringLiteral("-center"));
    element->setBackground(area);

    Union::BorderDefinition border;
    border.top = createLineDefinition(renderer, prefix + QStringLiteral("-top"));
    border.bottom = createLineDefinition(renderer, prefix + QStringLiteral("-bottom"));
    border.left = createLineDefinition(renderer, prefix + QStringLiteral("-left"));
    border.right = createLineDefinition(renderer, prefix + QStringLiteral("-right"));
    element->setBorder(border);

    Union::CornersDefinition corners;
    corners.topLeft = createCornerDefinition(renderer, prefix + u"-topleft"_qs);
    corners.topRight = createCornerDefinition(renderer, prefix + u"-topright"_qs);
    corners.bottomLeft = createCornerDefinition(renderer, prefix + u"-bottomleft"_qs);
    corners.bottomRight = createCornerDefinition(renderer, prefix + u"-bottomright"_qs);
    element->setCorners(corners);

    //     Union::ShadowDefinition shadow;
    //     element->setShadow(shadow);
    //
    //     Union

    return element;
}

std::optional<Union::LineDefinition> PlasmaSvgLoader::createLineDefinition(QSvgRenderer &renderer, const QString &elementName)
{
    auto image = renderElement(renderer, elementName);
    if (image) {
        Union::LineDefinition definition;
        definition.image = image;
        return definition;
    }

    return std::nullopt;
}

std::optional<Union::CornerDefinition> PlasmaSvgLoader::createCornerDefinition(QSvgRenderer &renderer, const QString &elementName)
{
    auto image = renderElement(renderer, elementName);
    if (image) {
        Union::CornerDefinition definition;
        definition.image = image;
        return definition;
    }

    return std::nullopt;
}

std::optional<Union::ImageDefinition> PlasmaSvgLoader::renderElement(QSvgRenderer &renderer, const QString &elementName)
{
    if (!renderer.elementExists(elementName)) {
        return std::nullopt;
    }

    auto bounds = renderer.transformForElement(elementName).map(renderer.boundsOnElement(elementName)).boundingRect();

    QImage image(bounds.size().toSize(), QImage::Format_ARGB32);
    QPainter painter(&image);
    renderer.render(&painter, elementName);

    Union::ImageDefinition result;
    result.imageData = image;
    result.width = Union::Size(image.width(), Union::Size::Unit::DevicePixels);
    result.height = Union::Size(image.height(), Union::Size::Unit::DevicePixels);

    return result;
}
