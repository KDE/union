// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "UnionStyle.h"
#include "StyleDrawing.h"
#include "StyleUtils.h"

#include <ElementQuery.h>
#include <QApplication>
#include <StyleRegistry.h>

#include <QImage>
#include <QPainter>
#include <QPushButton>
#include <QStyleFactory>
#include <QStyleOption>
#include <QWidget>

namespace
{

std::unique_ptr<Union::ElementQuery> executeQuery(const QList<Union::Element::Ptr> &elements)
{
    auto query = std::make_unique<Union::ElementQuery>(Union::StyleRegistry::instance()->defaultStyle());
    query->setElements(elements);
    query->execute();
    return query;
}

Union::Element::Ptr createCheckBoxElement(const QStyleOption *option)
{
    auto element = Union::Element::create();
    element->setType(QStringLiteral("CheckBox"));
    if (option) {
        element->setStates(statesFromOption(option));
    }

    if (const auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option); buttonOption && !buttonOption->icon.isNull()) {
        element->setHint(QStringLiteral("with-icon"));
    }

    return element;
}

Union::Element::Ptr createIndicatorElement(const Union::Element::Ptr &parentElement)
{
    auto indicator = Union::Element::create();
    indicator->setType(QStringLiteral("Indicator"));
    indicator->setStates(parentElement->states());
    return indicator;
}

QMargins marginsFromSizeGroup(const Union::Properties::SizePropertyGroup *size)
{
    if (!size) {
        return {};
    }

    return size->toMargins().toMargins();
}

QSize sizeFromMargins(const QMargins &margins)
{
    return {margins.left() + margins.right(), margins.top() + margins.bottom()};
}

QRect insetRect(const QRect &rect, const Union::Properties::StylePropertyGroup *properties)
{
    QRect result = rect;

    if (const auto layout = properties ? properties->layout() : nullptr; layout && layout->inset()) {
        result -= marginsFromSizeGroup(layout->inset());
    }

    return result;
}

QRect innerBackgroundRect(const QRect &rect, const Union::Properties::StylePropertyGroup *properties)
{
    QRectF innerRect = rect;

    if (const auto border = properties ? properties->border() : nullptr) {
        innerRect -= border->sizes();
    }

    return innerRect.toAlignedRect();
}

QSize styledIndicatorSize(const Union::Properties::StylePropertyGroup *properties, const QSize &fallback)
{
    QSize size = fallback;

    if (const auto layout = properties ? properties->layout() : nullptr) {
        if (const auto width = layout->width()) {
            size.setWidth(qRound(*width));
        }
        if (const auto height = layout->height()) {
            size.setHeight(qRound(*height));
        }
    }

    return size;
}

void drawStyledBackgroundImage(QPainter *painter, const QRect &rect, const Union::Properties::StylePropertyGroup *properties)
{
    const auto background = properties ? properties->background() : nullptr;
    const auto image = background ? background->image() : nullptr;
    if (!image || !image->source()) {
        return;
    }

    QImage renderedImage;
    const auto targetRect = innerBackgroundRect(rect, properties);
    const auto sourceImage = QImage(QString::fromStdString(image->source()->string()));

    if (sourceImage.isNull()) {
        return;
    }

    renderedImage = sourceImage.convertToFormat(QImage::Format_ARGB32_Premultiplied);

    if (const auto flags = image->flags();
        flags && (flags->testFlag(Union::Properties::ImageFlag::Mask) || flags->testFlag(Union::Properties::ImageFlag::InvertedMask))) {
        const auto maskColor = image->maskColor().value_or(Union::Color{}).toQColor();
        QPainter imagePainter(&renderedImage);
        imagePainter.setCompositionMode(flags->testFlag(Union::Properties::ImageFlag::InvertedMask) ? QPainter::CompositionMode_DestinationOut
                                                                                                    : QPainter::CompositionMode_SourceIn);
        imagePainter.fillRect(renderedImage.rect(), maskColor);
    }

    painter->drawImage(targetRect, renderedImage);
}

}

UnionStyle::UnionStyle()
    : QCommonStyle()
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
        // TODO: we may have to keep it as a QRectF here
        QRect rect = option->rect;
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
        QCommonStyle::drawControl(CE_PushButtonLabel, &labelOption, painter, widget);

        return;
    }
    QCommonStyle::drawControl(controlElement, option, painter, widget);
}

void UnionStyle::drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (element == PE_IndicatorCheckBox /*&& option && !option->state.testFlag(State_NoChange)*/) {
        auto checkboxElement = createCheckBoxElement(option);
        auto indicatorElement = createIndicatorElement(checkboxElement);

        const auto query = executeQuery({checkboxElement, indicatorElement});
        if (!query->hasMatches()) {
            QCommonStyle::drawPrimitive(element, option, painter, widget);
            return;
        }

        const auto properties = query->properties();
        const auto rect = insetRect(option->rect, properties);

        drawBackground(painter, rect, properties);
        drawStyledBackgroundImage(painter, rect, properties);
        return;
    }

    QCommonStyle::drawPrimitive(element, option, painter, widget);
}

QSize UnionStyle::sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *widget) const
{
    if (ct == CT_PushButton) {
        auto size = QCommonStyle::sizeFromContents(ct, opt, contentsSize, widget);

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
    if (ct == CT_CheckBox) {
        QSize size = contentsSize;

        auto checkboxElement = createCheckBoxElement(opt);
        const auto checkboxQuery = executeQuery({checkboxElement});
        const auto indicatorQuery = executeQuery({checkboxElement, createIndicatorElement(checkboxElement)});

        int spacing = pixelMetric(PM_CheckBoxLabelSpacing, opt, widget);
        QSize indicatorSize{pixelMetric(PM_IndicatorWidth, opt, widget), pixelMetric(PM_IndicatorHeight, opt, widget)};

        if (indicatorQuery->hasMatches()) {
            indicatorSize = styledIndicatorSize(indicatorQuery->properties(), indicatorSize);
        }

        const auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(opt);
        const bool hasLabelContent = buttonOption && (!buttonOption->text.isEmpty() || !buttonOption->icon.isNull());

        size.rwidth() += indicatorSize.width();
        if (hasLabelContent) {
            size.rwidth() += spacing;
        }
        size.setHeight(std::max(size.height(), indicatorSize.height()));

        if (checkboxQuery->hasMatches()) {
            const auto properties = checkboxQuery->properties();
            if (const auto layout = properties->layout()) {
                size += sizeFromMargins(marginsFromSizeGroup(layout->padding()));
                size += sizeFromMargins(marginsFromSizeGroup(layout->inset()));

                if (const auto width = layout->width()) {
                    size.setWidth(std::max(size.width(), qRound(*width)));
                }
                if (const auto height = layout->height()) {
                    size.setHeight(std::max(size.height(), qRound(*height)));
                }
            }
        }

        return size;
    }
    return QCommonStyle::sizeFromContents(ct, opt, contentsSize, widget);
}

int UnionStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
    switch (metric) {
    case PM_IndicatorWidth:
    case PM_IndicatorHeight: {
        auto checkboxElement = createCheckBoxElement(option);
        const auto query = executeQuery({checkboxElement, createIndicatorElement(checkboxElement)});
        if (query->hasMatches()) {
            const auto styledSize = styledIndicatorSize(
                query->properties(),
                {QCommonStyle::pixelMetric(PM_IndicatorWidth, option, widget), QCommonStyle::pixelMetric(PM_IndicatorHeight, option, widget)});
            return metric == PM_IndicatorWidth ? styledSize.width() : styledSize.height();
        }
        break;
    }
    case PM_CheckBoxLabelSpacing: {
        const auto query = executeQuery({createCheckBoxElement(option)});
        if (query->hasMatches()) {
            if (const auto layout = query->properties()->layout(); layout && layout->spacing()) {
                return qRound(*layout->spacing());
            }
        }
        break;
    }
    // Don't shift button text when sunken
    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:
        return 0;
    default:
        return QCommonStyle::pixelMetric(metric, option, widget);
    }

    return QCommonStyle::pixelMetric(metric, option, widget);
}

void UnionStyle::polish(QApplication *application)
{
    QCommonStyle::polish(application);

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
    QCommonStyle::polish(widget);

    if (qobject_cast<QPushButton *>(widget)) {
        widget->setAttribute(Qt::WA_Hover);
    }
}
