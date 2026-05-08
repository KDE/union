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
        element->setStates(buttonStatesFromOption(option));
        element->setColorSet(Union::Element::ColorSet::Button);
        element->setHint(QStringLiteral("flat"), buttonOption->features.testFlag(QStyleOptionButton::ButtonFeature::Flat));

        const auto properties = prepareProperties(element);
        auto rect = prepareRectangle(option, properties).toRect();
        drawBackground(painter, rect, properties);

        QStyleOptionButton labelOption(*buttonOption);
        labelOption.palette.setColor(QPalette::ButtonText, properties->text()->color().value().toQColor());

        QProxyStyle::drawControl(CE_PushButtonLabel, &labelOption, painter, widget);

        return;
    }
    QProxyStyle::drawControl(controlElement, option, painter, widget);
}

void UnionStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    if (control == CC_ToolButton) {
        const auto buttonOption = static_cast<const QStyleOptionToolButton *>(option);

        auto element = Union::Element::create();
        element->setType(QStringLiteral("ToolButton"));
        element->setStates(buttonStatesFromOption(option));
        element->setColorSet(Union::Element::ColorSet::Button);
        // QStyle has no flat toolbuttons
        element->setHint(QStringLiteral("raised"), true);

        const auto properties = prepareProperties(element);
        auto rect = prepareRectangle(option, properties).toRect();
        drawBackground(painter, rect, properties);

        QStyleOptionToolButton labelOption(*buttonOption);
        labelOption.palette.setColor(QPalette::ButtonText, properties->text()->color().value().toQColor());

        QProxyStyle::drawControl(CE_ToolButtonLabel, &labelOption, painter, widget);
        return;
    }

    QProxyStyle::drawComplexControl(control, option, painter, widget);
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
