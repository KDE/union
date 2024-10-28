/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "PropertyFunctions.h"

#include <QPainter>
#include <QVariant>

#include "PlasmaSvgRenderer.h"

#include "plasmasvg_logging.h"

using namespace PropertyFunctions;

PropertyFunctionResult PropertyFunctions::elementSize(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto renderer = PlasmaSvgRenderer::rendererForPath(context.theme, context.path(), QPalette::Normal, context.colorSet());
    if (!renderer) {
        return Error{"Could not find renderer"};
    }

    const auto element = context.prefixedElementName();
    if (!renderer->elementExists(element)) {
        return Error{u"Could not find element " + element};
    }

    auto size = renderer->elementRect(element).size();
    with_child(node, "invert", [&](auto node) {
        if (value<bool>(node)) {
            size = QSizeF(-size.width(), -size.height());
        }
    });

    return QVariant{size};
}

PropertyFunctionResult PropertyFunctions::elementWidth(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto size = elementSize(node, context);
    return size.has_value() ? PropertyFunctionResult{size.value<QSizeF>().width()} : size.error();
}

PropertyFunctionResult PropertyFunctions::elementHeight(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto size = elementSize(node, context);
    return size.has_value() ? PropertyFunctionResult{size.value<QSizeF>().height()} : size.error();
}

PropertyFunctionResult PropertyFunctions::elementImage(ryml::ConstNodeRef node, LoadingContext &context)
{
    Q_UNUSED(node)
    const auto element = context.prefixedElementName();

    auto renderer = PlasmaSvgRenderer::rendererForPath(context.theme, context.path(), QPalette::Normal, context.colorSet());
    if (!renderer) {
        return Error{"Could not find renderer"};
    }

    if (!renderer->elementExists(element)) {
        return Error{u"Could not find element " + element};
    }

    auto size = renderer->elementRect(element).size();

    QImage image(size.toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    renderer->render(&painter, element);

    return image;
}

PropertyFunctionResult PropertyFunctions::elementImageBlend(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto elementsNode = node.find_child("elements");
    if (!elementsNode.readable()) {
        return Error("Could not find key 'elements'");
    }

    if (!elementsNode.has_children()) {
        return Error("Key 'elements' is empty");
    }

    QList<QImage> images;
    int maxWidth = 0;
    int maxHeight = 0;
    for (auto child : elementsNode.children()) {
        context.data.elementNames.push(value<QString>(child));
        auto image = elementImage(node, context);
        context.data.elementNames.pop();

        if (image.has_value()) {
            auto imageValue = image.value<QImage>();
            images.append(imageValue);
            maxWidth = std::max(maxWidth, imageValue.width());
            maxHeight = std::max(maxHeight, imageValue.height());
        } else {
            return image.error();
        }
    }

    Qt::Alignment align;
    with_child(node, "align", [&](auto node) {
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

PropertyFunctionResult PropertyFunctions::sum(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto elementsNode = node.find_child("elements");
    if (!elementsNode.readable()) {
        return Error{"Could not find key 'elements'"};
    }

    if (!elementsNode.has_children()) {
        return Error{"Key 'elements' is empty"};
    }

    auto cleanup = context.pushFromNode(node);

    QByteArray function;
    with_child(node, "function", [&function](auto node) {
        function = value<QByteArray>(node);
    });

    auto func = propertyFunctions.value(function);

    qreal result = 0.0;
    for (auto child : elementsNode.children()) {
        if (child.has_val()) {
            if (func) {
                context.data.elementNames.push(value<QString>(child));
                auto value = func(child, context);
                if (value.has_value()) {
                    result += value.value<qreal>();
                } else {
                    return value.error();
                }
                context.data.elementNames.pop();
            }
        } else if (child.is_map()) {
            auto value = elementProperty<qreal>(child, context);
            if (value.has_value()) {
                result += value.value();
            } else {
                return value.error();
            }
        }
    }

    return result;
}
