// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "UnionStyle.h"
#include "StyleDrawing.h"
#include "StyleUtils.h"

#include <ElementQuery.h>
#include <QApplication>
#include <StyleRegistry.h>

#include <QPainter>
#include <QPushButton>
#include <QStyleFactory>
#include <QStyleOption>
#include <QWidget>

UnionStyle::UnionStyle()
    : QProxyStyle(qEnvironmentVariable("UNION_WIDGETS_BASE_STYLE", QStringLiteral("breeze")))
{
    Union::StyleRegistry::instance()->load();
}

void UnionStyle::drawControl(QStyle::ControlElement controlElement, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    // Make lines not look completely terrible on fractional scales
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (controlElement == CE_PushButton) {
        const auto buttonOption = static_cast<const QStyleOptionButton *>(option);

        auto element = Union::Element::create();
        element->setType(QStringLiteral("Button"));
        element->setStates(statesFromOption(option));
        element->setColorSet(Union::Element::ColorSet::Button);
        element->setHint(QStringLiteral("flat"), buttonOption->features.testFlag(QStyleOptionButton::ButtonFeature::Flat));

        const auto style = Union::StyleRegistry::instance()->defaultStyle();
        const auto query = std::make_unique<Union::ElementQuery>(style);

        query->setElements({element});
        query->execute();

        const auto properties = query->properties();

        // Shrink the widget rect by the insets
        QRectF rect = option->rect.toRectF();
        if (const auto layout = properties->layout()) {
            if (layout->inset()) {
                rect -= layout->inset()->toMargins().toMargins();
            }
        }

        // Make sure to take out the space left for the visual focus rect
        rect.setLeft(rect.left() + 2);
        rect.setRight(rect.right() - 2);
        rect.setTop(rect.top() + 2);
        rect.setBottom(rect.bottom() - 2);

        drawBackground(painter, rect, properties);
        QStyleOptionButton labelOption(*buttonOption);
        labelOption.palette.setColor(QPalette::ButtonText, properties->text()->color().value().toQColor());

        // Draw label
        // TODO: union-ize better
        UnionStyle::drawControl(CE_PushButtonLabel, &labelOption, painter, widget);

        return;
    } else if (controlElement == CE_PushButtonLabel) {
        auto element = Union::Element::create();
        element->setType(QStringLiteral("Button"));
        element->setStates(statesFromOption(option));
        element->setColorSet(Union::Element::ColorSet::Button);
        const auto buttonOption = static_cast<const QStyleOptionButton *>(option);

        const auto style = Union::StyleRegistry::instance()->defaultStyle();
        const auto query = std::make_unique<Union::ElementQuery>(style);

        query->setElements({element});
        query->execute();
        const auto properties = query->properties();

        auto horizontalAlignment = toQtHorizontal(properties->text()->alignment()->horizontal().value_or(Union::Properties::Alignment::Unspecified));

        auto verticalAlignment = toQtVertical(properties->text()->alignment()->vertical().value_or(Union::Properties::Alignment::Unspecified));

        auto mainRect = buttonOption->rect;
        auto textRect = mainRect;
        auto iconRect = mainRect;

        if (!buttonOption->icon.isNull()) {
            auto iconHorizontalAlign = toQtHorizontal(properties->icon()->alignment()->horizontal().value_or(Union::Properties::Alignment::Unspecified));
            auto iconVerticalAlign = toQtVertical(properties->icon()->alignment()->vertical().value_or(Union::Properties::Alignment::Unspecified));

            qreal spacing = properties->layout()->spacing().value_or(0);
            qreal width = properties->icon()->width().value_or(0);
            iconRect.setWidth(width);
            iconRect.setHeight(properties->icon()->height().value_or(0));

            // Create something that takes any amount of elements, a rectangle and then creates
            // list of needed amount of rectangles in same order, but they fit inside the given rectangle
            // then this can be used to draw things

            auto textOrder = properties->text()->alignment()->order().value_or(0);
            auto iconOrder = properties->icon()->alignment()->order().value_or(0);

            iconRect.moveCenter(QPoint(iconRect.center().x() + spacing, mainRect.center().y()));

            auto pixmap = buttonOption->icon.pixmap(QSize(properties->icon()->width().value_or(0), properties->icon()->height().value_or(0)));
            painter->drawPixmap(iconRect, pixmap);
        }

        QTextOption textOption;
        textOption.setAlignment(horizontalAlignment | verticalAlignment);

        // QProxyStyle::visualAlignment(Qt::LayoutDirectionAuto, horizontalAlignment | verticalAlignment);

        painter->setPen(buttonOption->palette.buttonText().color());
        painter->drawText(textRect, buttonOption->text, textOption);

        return;
    }
    QProxyStyle::drawControl(controlElement, option, painter, widget);
}

QSize UnionStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *widget) const
{
    if (ct == CT_PushButton) {
        auto size = QProxyStyle::sizeFromContents(ct, opt, contentsSize, widget);

        auto element = Union::Element::create();
        element->setType(QStringLiteral("Button"));
        element->setColorSet(Union::Element::ColorSet::Button);

        const auto style = Union::StyleRegistry::instance()->defaultStyle();
        const auto matches = style->matches({element});
        const auto properties = matches.first()->properties();

        if (const auto layout = properties->layout()) {
            const QMargins padding = layout->padding()->toMargins().toMargins();
            size = size.grownBy(padding / 2.0f);
        }

        // Make sure to allocate space for the visual focus rect
        size = size.grownBy(QMargins(2, 2, 2, 2));

        return size;
    }
    return QProxyStyle::sizeFromContents(ct, opt, contentsSize, widget);
}

int UnionStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric) {
    // Don't shift button text when sunken
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
        return 0;
    default:
        return QProxyStyle::pixelMetric(metric, option, widget);
    }
}

void UnionStyle::polish(QApplication *application)
{
    QProxyStyle::polish(application);

    // Set global window color
    auto element = Union::Element::create();
    element->setType(QStringLiteral("ApplicationWindow"));

    const auto style = Union::StyleRegistry::instance()->defaultStyle();
    const auto matches = style->matches({element});
    const auto properties = matches.first()->properties();

    QPalette palette;
    palette.setColor(QPalette::Window, properties->background()->color().value().toQColor());

    application->setPalette(palette);
}

void UnionStyle::polish(QWidget *widget)
{
    QProxyStyle::polish(widget);

    if (qobject_cast<QPushButton *>(widget)) {
        widget->setAttribute(Qt::WA_Hover);
    }
}
