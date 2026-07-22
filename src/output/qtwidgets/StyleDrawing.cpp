// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "StyleDrawing.h"

#include "StyleUtils.h"
#include <Element.h>
#include <ElementQuery.h>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <StyleRegistry.h>
#include <StyleUtils.h>
#include <algorithm>

#include "LruCache.h"

static Union::LruImageCache imageCache;

void drawBackground(QPainter *painter, const QRect &rect, const Union::Properties::StylePropertyGroup *style)
{
    QRectF innerRect = rect;
    // Borders
    QMarginsF borderSizes = {0, 0, 0, 0};
    bool allBordersEqual = false;
    bool allBorderColorsEqual = false;

    if (const auto border = style->border()) {
        borderSizes = border->sizes();
        allBordersEqual = qFuzzyCompare(borderSizes.left(), borderSizes.right()) && qFuzzyCompare(borderSizes.top(), borderSizes.bottom())
            && qFuzzyCompare(borderSizes.left(), borderSizes.top());

        if (border->top() && border->left() && border->right() && border->bottom()) {
            allBorderColorsEqual = (border->left()->color() == border->right()->color()) && (border->top()->color() == border->bottom()->color())
                && (border->left()->color() == border->top()->color());
        }

        // Make space for outlines
        innerRect -= borderSizes;
    }

    // Corners
    Union::Properties::CornersPropertyGroup::CornerRadii cornerRadii = {0, 0, 0, 0};
    Union::Properties::CornersPropertyGroup::CornerRadii constrainedRadii = {0, 0, 0, 0};
    Union::Properties::CornersPropertyGroup::CornerRadii innerCornerRadii = {0, 0, 0, 0};
    bool allCornerRadiiEqual = false;

    if (const auto corners = style->corners()) {
        cornerRadii = corners->radii();
        constrainedRadii = constrainRadii(rect, cornerRadii);
        allCornerRadiiEqual = qFuzzyCompare(constrainedRadii.topLeft, constrainedRadii.topRight)
            && qFuzzyCompare(constrainedRadii.bottomLeft, constrainedRadii.bottomRight)
            && qFuzzyCompare(constrainedRadii.topLeft, constrainedRadii.bottomRight);
        innerCornerRadii = constrainedRadii;

        // Make space for outlines
        // Shrink inner radii
        if (style->border()) {
            innerCornerRadii = {std::max(innerCornerRadii.topLeft - std::min(borderSizes.top(), borderSizes.left()), 0.0),
                                std::max(innerCornerRadii.topRight - std::min(borderSizes.top(), borderSizes.right()), 0.0),
                                std::max(innerCornerRadii.bottomLeft - std::min(borderSizes.bottom(), borderSizes.left()), 0.0),
                                std::max(innerCornerRadii.bottomRight - std::min(borderSizes.bottom(), borderSizes.right()), 0.0)};
        }
    }

    // Draw background
    if (const auto background = style->background()) {
        QPainterPath path;

        // Draw less complex rectangles if complex ones are not needed
        if (allCornerRadiiEqual && !constrainedRadii.topLeft) {
            path.addRect(innerRect);
        } else if (allCornerRadiiEqual) {
            path.addRoundedRect(innerRect, innerCornerRadii.topLeft, innerCornerRadii.topLeft);
        } else {
            path = unevenRadiiRectPath(innerRect, innerCornerRadii);
        }

        if (const auto color = background->color()) {
            painter->setPen(Qt::transparent);
            painter->setBrush(color.value().toQColor());
            painter->drawPath(path);
        }
        if (const auto image = background->image(); image && image->source()) {
            painter->save();
            painter->setClipPath(path);

            QSizeF imageSize;
            imageSize.setHeight(style->layout()->height().value_or(16));
            imageSize.setWidth(style->layout()->width().value_or(16));
            auto loadedImage = imageCache.load(image->source().value(), imageSize);
            if (image->flags().value().testAnyFlag(Union::Properties::ImageFlag::Mask)) {
                painter->setBrush(Qt::transparent);
                painter->setPen(image->maskColor()->toQColor());
                QBitmap mask = QBitmap::fromImage(loadedImage.createAlphaMask());
                auto imageRect = innerRect;
                imageRect.setWidth(mask.width());
                imageRect.setHeight(mask.height());
                imageRect.moveCenter(innerRect.center());
                painter->drawPixmap(imageRect.topLeft(), mask);
            } else {
                painter->drawImage(innerRect, loadedImage);
            }

            painter->restore();
        }
    }
    // Draw borders and corners
    if (const auto border = style->border()) {
        painter->setRenderHint(QPainter::Antialiasing, true);
        // Make simpler border shapes if complex ones are not necessary
        if (allBordersEqual && allBorderColorsEqual && allCornerRadiiEqual) { // All radii and borders identical
            QPainterPath rectangularOutline;
            QPainterPath innerRectangularOutline;

            innerRectangularOutline.addRoundedRect(innerRect, innerCornerRadii.topLeft, innerCornerRadii.topLeft);
            rectangularOutline.addRoundedRect(rect, constrainedRadii.topLeft, constrainedRadii.topLeft);

            rectangularOutline = rectangularOutline.subtracted(innerRectangularOutline);

            painter->setPen(Qt::transparent);
            painter->setBrush(border->top()->color().value().toQColor());
            painter->drawPath(rectangularOutline);
        } else if (allBordersEqual && allBorderColorsEqual) { // All borders identical
            QPainterPath rectangularOutline;
            QPainterPath innerRectangularOutline;

            innerRectangularOutline = unevenRadiiRectPath(innerRect, innerCornerRadii);
            rectangularOutline = unevenRadiiRectPath(rect, constrainedRadii);

            rectangularOutline = rectangularOutline.subtracted(innerRectangularOutline);

            painter->setPen(Qt::transparent);
            painter->setBrush(border->top()->color().value().toQColor());
            painter->drawPath(rectangularOutline);
        } else { // All other complex scenarios
            const auto corners = style->corners();
            // Draw borders
            if (const auto top = border->top()) {
                drawLineProperty(painter, rect, SubNodeIndex::Top, borderSizes, top, corners);
            }
            if (const auto right = border->right()) {
                drawLineProperty(painter, rect, SubNodeIndex::Right, borderSizes, right, corners);
            }
            if (const auto bottom = border->bottom()) {
                drawLineProperty(painter, rect, SubNodeIndex::Bottom, borderSizes, bottom, corners);
            }
            if (const auto left = border->left()) {
                drawLineProperty(painter, rect, SubNodeIndex::Left, borderSizes, left, corners);
            }

            // Draw corners if at least one border is present
            if (borderSizes.top() || borderSizes.left()) {
                const auto topLeft = corners ? corners->topLeft() : nullptr;
                drawCornerProperty(painter, rect, SubNodeIndex::TopLeft, border, topLeft);
            }
            if (borderSizes.top() || borderSizes.right()) {
                const auto topRight = corners ? corners->topRight() : nullptr;
                drawCornerProperty(painter, rect, SubNodeIndex::TopRight, border, topRight);
            }
            if (borderSizes.bottom() || borderSizes.right()) {
                const auto bottomRight = corners ? corners->bottomRight() : nullptr;
                drawCornerProperty(painter, rect, SubNodeIndex::BottomRight, border, bottomRight);
            }
            if (borderSizes.bottom() || borderSizes.left()) {
                const auto bottomLeft = corners ? corners->bottomLeft() : nullptr;
                drawCornerProperty(painter, rect, SubNodeIndex::BottomLeft, border, bottomLeft);
            }
        }
    }
}

QPainterPath unevenRadiiRectPath(const auto &rect, const Union::Properties::CornersPropertyGroup::CornerRadii cornerRadii)
{
    QPainterPath path;

    // Path
    path.moveTo(rect.x(), rect.y() + rect.height() - cornerRadii.bottomLeft);

    // Top Left
    path.lineTo(rect.x(), rect.y() + cornerRadii.topLeft);
    if (cornerRadii.topLeft) {
        path.arcTo(QRectF(rect.x(), rect.y(), cornerRadii.topLeft * 2, cornerRadii.topLeft * 2), 180, -90);
    }

    // Top Right
    path.lineTo(rect.x() + rect.width() - cornerRadii.topRight, rect.top());
    if (cornerRadii.topRight) {
        path.arcTo(QRectF(rect.x() + rect.width() - cornerRadii.topRight * 2, rect.y(), cornerRadii.topRight * 2, cornerRadii.topRight * 2), 90, -90);
    }

    // Bottom Right
    path.lineTo(rect.x() + rect.width(), rect.y() + rect.height() - cornerRadii.bottomRight);
    if (cornerRadii.bottomRight) {
        path.arcTo(QRectF(rect.x() + rect.width() - cornerRadii.bottomRight * 2,
                          rect.y() + rect.height() - cornerRadii.bottomRight * 2,
                          cornerRadii.bottomRight * 2,
                          cornerRadii.bottomRight * 2),
                   0,
                   -90);
    }

    // Bottom Left
    path.lineTo(rect.x() + cornerRadii.bottomLeft, rect.y() + rect.height());
    if (cornerRadii.bottomLeft) {
        path.arcTo(QRectF(rect.x(), rect.y() + rect.height() - cornerRadii.bottomLeft * 2, cornerRadii.bottomLeft * 2, cornerRadii.bottomLeft * 2), -90, -90);
    }

    path.closeSubpath();

    return path;
}

Union::Properties::CornersPropertyGroup::CornerRadii constrainRadii(const QRect &rect, const Union::Properties::CornersPropertyGroup::CornerRadii cornerRadii)
{
    auto topLeft = std::min({rect.width() / 2.0, rect.height() / 2.0, cornerRadii.topLeft});
    auto topRight = std::min({rect.width() / 2.0, rect.height() / 2.0, cornerRadii.topRight});
    auto bottomLeft = std::min({rect.width() / 2.0, rect.height() / 2.0, cornerRadii.bottomLeft});
    auto bottomRight = std::min({rect.width() / 2.0, rect.height() / 2.0, cornerRadii.bottomRight});

    Union::Properties::CornersPropertyGroup::CornerRadii constrainedRadii = {std::max(topLeft, 0.0),
                                                                             std::max(topRight, 0.0),
                                                                             std::max(bottomLeft, 0.0),
                                                                             std::max(bottomRight, 0.0)};

    return constrainedRadii;
}

void drawLineProperty(QPainter *painter,
                      const QRect &rect,
                      SubNodeIndex subNodeIndex,
                      const QMarginsF &borderSizes,
                      const Union::Properties::LinePropertyGroup *line,
                      const Union::Properties::CornersPropertyGroup *corners)
{
    auto cornerRadii = corners ? constrainRadii(rect, corners->radii()) : Union::Properties::CornersPropertyGroup::CornerRadii{};

    QRectF lineRect;
    switch (subNodeIndex) {
    case SubNodeIndex::Left: {
        const qreal topMargin = cornerRadii.topLeft;
        const qreal bottomMargin = cornerRadii.bottomLeft;

        lineRect = QRectF{static_cast<double>(rect.x()),
                          rect.y() + borderSizes.top() + topMargin,
                          line->size().value_or(0.0),
                          rect.height() - borderSizes.top() - borderSizes.bottom() - bottomMargin - topMargin};
    } break;
    case SubNodeIndex::Right: {
        const qreal topMargin = cornerRadii.topRight;
        const qreal bottomMargin = cornerRadii.bottomRight;

        lineRect = QRectF{rect.x() + rect.width() - line->size().value_or(0.0),
                          rect.y() + borderSizes.top() + topMargin,
                          line->size().value_or(0.0),
                          rect.height() - borderSizes.top() - borderSizes.bottom() - bottomMargin - topMargin};
    } break;
    case SubNodeIndex::Top: {
        const qreal leftMargin = cornerRadii.topLeft;
        const qreal rightMargin = cornerRadii.topRight;

        lineRect = QRectF{rect.x() + borderSizes.left() + leftMargin,
                          static_cast<double>(rect.y()),
                          rect.width() - borderSizes.left() - borderSizes.right() - rightMargin - leftMargin,
                          line->size().value_or(0.0)};
    } break;
    case SubNodeIndex::Bottom: {
        const qreal leftMargin = cornerRadii.bottomLeft;
        const qreal rightMargin = cornerRadii.bottomRight;

        lineRect = QRectF{rect.x() + borderSizes.left() + leftMargin,
                          rect.y() + rect.height() - line->size().value_or(0.0),
                          rect.width() - borderSizes.left() - borderSizes.right() - rightMargin - leftMargin,
                          line->size().value_or(0.0)};
    } break;
    default:
        Q_UNREACHABLE();
    }

    if (auto color = line->color()) {
        painter->setPen(Qt::transparent);
        painter->setBrush(color.value().toQColor());
        painter->drawRect(lineRect);
    }
}

// TODO: Fix tiny gaps caused by antialiasing between corners and borders
void drawCornerProperty(QPainter *painter,
                        const QRect &rect,
                        SubNodeIndex subNodeIndex,
                        const Union::Properties::BorderPropertyGroup *border,
                        const Union::Properties::CornerPropertyGroup *corner)
{
    const auto borderSizes = border->sizes();

    QPainterPath path;
    QLinearGradient cornerTransition;
    const auto radius =
        corner ? std::min<double>({static_cast<double>(rect.width()) / 2, static_cast<double>(rect.height()) / 2, corner->radius().value_or(0.0)}) : 0.0;
    double innerRadius = 0.0;
    // Start, end, top and left assume a top-left corner being drawn clockwise which gets rotated
    double topMargin = 0.0;
    double leftMargin = 0.0;
    QColor startColor = Qt::transparent;
    QColor endColor = Qt::transparent;

    painter->setPen(Qt::transparent);
    painter->save();

    switch (subNodeIndex) {
    case SubNodeIndex::TopLeft: {
        leftMargin = borderSizes.left();
        topMargin = borderSizes.top();

        startColor = border->left() ? border->left()->color().value().toQColor() : Qt::transparent;
        endColor = border->top() ? border->top()->color().value().toQColor() : Qt::transparent;

        painter->translate(rect.x(), rect.y());
    } break;

    case SubNodeIndex::TopRight: {
        leftMargin = borderSizes.top();
        topMargin = borderSizes.right();

        startColor = border->top() ? border->top()->color().value().toQColor() : Qt::transparent;
        endColor = border->right() ? border->right()->color().value().toQColor() : Qt::transparent;

        painter->translate(rect.x() + rect.width(), rect.y());
        painter->rotate(90);
    } break;

    case SubNodeIndex::BottomRight: {
        leftMargin = borderSizes.right();
        topMargin = borderSizes.bottom();

        startColor = border->right() ? border->right()->color().value().toQColor() : Qt::transparent;
        endColor = border->bottom() ? border->bottom()->color().value().toQColor() : Qt::transparent;

        painter->translate(rect.x() + rect.width(), rect.y() + rect.height());
        painter->rotate(180);
    } break;

    case SubNodeIndex::BottomLeft: {
        leftMargin = borderSizes.bottom();
        topMargin = borderSizes.left();

        startColor = border->bottom() ? border->bottom()->color().value().toQColor() : Qt::transparent;
        endColor = border->left() ? border->left()->color().value().toQColor() : Qt::transparent;

        painter->translate(rect.x(), rect.y() + rect.height());
        painter->rotate(270);
    } break;

    default: {
        break;
    }
    }

    innerRadius = std::max(radius - std::min(topMargin, leftMargin), 0.0);

    // Draw path only when necessary, otherwise draw a simple rectangle
    if (radius) {
        path.moveTo(radius + leftMargin, 0);

        path.lineTo(radius, 0);
        path.arcTo(QRectF(0, 0, radius * 2, radius * 2), 90, 90);
        path.lineTo(0, radius + topMargin);
        path.lineTo(leftMargin, radius + topMargin);
        path.lineTo(leftMargin, innerRadius + topMargin);
        if (innerRadius) {
            path.arcTo(QRectF(leftMargin, topMargin, innerRadius * 2, innerRadius * 2), 180, -90);
        }
        path.lineTo(leftMargin + radius, topMargin);
        path.lineTo(leftMargin + radius, 0);
        path.lineTo(radius, 0);

        path.closeSubpath();
    } else {
        path.addRect(QRectF(0, 0, leftMargin, topMargin));
    }

    if (startColor == endColor) {
        painter->setBrush(startColor);
    } else {
        cornerTransition = QLinearGradient(QPointF(0, radius + topMargin), QPointF(radius + leftMargin, 0));
        cornerTransition.setColorAt(0, startColor);
        cornerTransition.setColorAt(1, endColor);
        painter->setBrush(QBrush(cornerTransition));
    }

    painter->drawPath(path);
    painter->restore();
}

void drawElement(Union::Properties::StylePropertyGroup *properties, QPainter *painter, const QStyleOption *opt)
{
    drawBackground(painter, opt->rect, properties);
}

void drawIconText(const QStyleOption *opt, const QStyle *qstyle, QPainter *painter, const QWidget *widget, const QIcon &icon, const QString &text)
{
    QList<Union::Element::Ptr> elements = prepareElements(opt, widget);
    QStringList elementTypes;

    bool hasIcon = !icon.isNull();
    bool hasText = !text.isEmpty();

    const bool enabled = opt->state.testFlag(QStyle::State_Enabled);
    auto properties = queryProperties(elements);

    QMargins paddings;
    if (properties->layout()->padding()) {
        paddings = properties->layout()->padding()->toMargins().toMargins();
    }

    QStringList subElements;
    if (hasIcon) {
        subElements.append(QStringLiteral("Icon"));
    }
    if (hasText) {
        subElements.append(QStringLiteral("Text"));
    }
    // Nothing to draw, just return
    if (subElements.isEmpty()) {
        return;
    }
    auto map = layoutMap(elements, opt, subElements);

    QRect textRect = map[QStringLiteral("Text")].rect.toRect();
    if (hasText) {
        // TODO: hide mnemonics if requested
        int flags = Qt::AlignVCenter;
        auto textAlignment = toQtAlignment(properties->text()->alignment());
        auto textFlags = toQtWrapMode(properties->text()->wrapMode().value_or(Union::Properties::TextWrapMode::NoWrap));
        auto textElide = toQtElideMode(properties->text()->elide().value_or(Union::Properties::TextElide::Right));
        auto textColor = properties->text()->color();
        flags |= textAlignment;
        flags |= textFlags;
        flags |= textElide;
        flags |= Qt::TextShowMnemonic;
        QColor penColor = opt->palette.text().color();
        if (textColor) {
            penColor = textColor->toQColor();
        }
        painter->save();
        painter->setPen(penColor);
        qstyle->drawItemText(painter, textRect, flags, opt->palette, enabled, text);
        painter->restore();
    }

    QRect iconRect = map[QStringLiteral("Icon")].rect.toRect();
    if (hasIcon) {
        auto iconAlignment = toQtAlignment(properties->icon()->alignment());
        auto iconColor = properties->icon()->color();
        const QPalette activePalette = opt->palette;
        const qreal dpr = painter->device() ? painter->device()->devicePixelRatioF() : qApp->devicePixelRatio();
        const QPixmap pixmap = icon.pixmap(iconRect.size(), dpr, enabled ? QIcon::Normal : QIcon::Disabled);
        QColor penColor = opt->palette.text().color(); // Use text color as fallback
        if (iconColor) {
            penColor = iconColor->toQColor();
        }
        painter->save();
        painter->setPen(penColor);
        qstyle->drawItemPixmap(painter, iconRect, iconAlignment, pixmap);
        painter->restore();
    }

    /*
    painter->save();
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::blue);
    painter->drawRect(opt->rect);
    painter->setPen(Qt::magenta);
    painter->drawRect(iconRect);
    painter->setPen(Qt::yellow);
    painter->drawRect(textRect);
    painter->restore();
    */
}
