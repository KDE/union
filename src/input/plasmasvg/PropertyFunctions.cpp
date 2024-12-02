/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "PropertyFunctions.h"

#include <QPainter>
#include <QVariant>

#include <KConfigGroup>
#include <KSharedConfig>

#include "PlasmaSvgRenderer.h"

#include "plasmasvg_logging.h"

using namespace PropertyFunctions;

PropertyFunctionResult PropertyFunctions::elementSize(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto renderer = PlasmaSvgRenderer::rendererForPath(context.theme, context.path(), context.colorGroup(), context.colorSet());
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

    auto renderer = PlasmaSvgRenderer::rendererForPath(context.theme, context.path(), context.colorGroup(), context.colorSet());
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

PropertyFunctionResult PropertyFunctions::fontFromName(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto cleanup = context.pushFromNode(node);

    QByteArrayView name;
    with_child(node, "name", [&](auto node) {
        name = value<QByteArrayView>(node);
    });

    if (name.isEmpty()) {
        return Error{"Could not find key 'name'"};
    }

    auto config = KSharedConfig::openConfig(QStringLiteral("kdeglobals"));
    auto group = config->group(QStringLiteral("General"));

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
        return Error{"Unknown font name: " + name};
    }

    return font;
}

PropertyFunctionResult PropertyFunctions::iconSizeFromName(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto cleanup = context.pushFromNode(node);

    QByteArrayView name;
    with_child(node, "name", [&](auto node) {
        name = value<QByteArrayView>(node);
    });

    if (name.isEmpty()) {
        return Error{"Could not find key 'name'"};
    }

    if (name == "small") {
        return 16;
    } else if (name == "small-medium") {
        return 22;
    } else if (name == "medium") {
        return 32;
    } else if (name == "large") {
        return 48;
    } else if (name == "huge") {
        return 64;
    } else if (name == "enormous") {
        return 128;
    } else {
        return Error{"Invalid name given for icon size: " + name};
    }
}

PropertyFunctionResult PropertyFunctions::valueFromName(ryml::ConstNodeRef node, LoadingContext &context)
{
    auto cleanup = context.pushFromNode(node);

    QByteArrayView name;
    with_child(node, "name", [&](auto node) {
        name = value<QByteArrayView>(node);
    });

    if (name.isEmpty()) {
        return Error{"Could not find key 'name'"};
    }

    qreal multiplier = 1.0;
    with_child(node, "multiplier", [&](auto node) {
        multiplier = value<qreal>(node);
    });

    qreal value = 0.0;
    if (name == "small-spacing") {
        value = 4.0;
    } else if (name == "medium-spacing") {
        value = 6.0;
    } else if (name == "large-spacing") {
        value = 8.0;
    } else if (name == "grid-unit") {
        value = 18.0;
    } else {
        return Error("Invalid name given for named value: " + name);
    }

    return value * multiplier;
}
