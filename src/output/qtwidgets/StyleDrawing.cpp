// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#include "StyleDrawing.h"

#include <QPainter>

#include "LruCache.h"

static Union::LruImageCache imageCache;

void drawBackground(QPainter *painter, const QRect &rect, const Union::Properties::StyleProperty *style)
{
    QMarginsF borderSizes;
    if (const auto border = style->border()) {
        borderSizes = border->sizes();
    }

    if (const auto background = style->background()) {
        auto innerRect = rect - borderSizes;
        if (const auto image = background->image(); image && image->source()) {
            painter->drawImage(innerRect, imageCache.load(image->source().value(), innerRect.size()));
        }
        if (const auto color = background->color()) {
            painter->setPen(Qt::transparent);
            painter->setBrush(color.value().toQColor());
            painter->drawRect(innerRect);
        }
    }

    // Draw borders
    if (const auto border = style->border()) {
        const auto corners = style->corners();
        if (const auto left = border->left()) {
            drawLineProperty(painter, rect, SubNodeIndex::Left, borderSizes, left, corners);
        }
        if (const auto right = border->right()) {
            drawLineProperty(painter, rect, SubNodeIndex::Right, borderSizes, right, corners);
        }
        if (const auto top = border->top()) {
            drawLineProperty(painter, rect, SubNodeIndex::Top, borderSizes, top, corners);
        }
        if (const auto bottom = border->bottom()) {
            drawLineProperty(painter, rect, SubNodeIndex::Bottom, borderSizes, bottom, corners);
        }
    }

    // Draw corners
    if (auto corners = style->corners()) {
        if (const auto topLeft = corners->topLeft()) {
            drawCornerProperty(painter, rect, SubNodeIndex::TopLeft, borderSizes, topLeft);
        }
        if (const auto topRight = corners->topRight()) {
            drawCornerProperty(painter, rect, SubNodeIndex::TopRight, borderSizes, topRight);
        }
        if (const auto bottomLeft = corners->bottomLeft()) {
            drawCornerProperty(painter, rect, SubNodeIndex::BottomLeft, borderSizes, bottomLeft);
        }
        if (const auto bottomRight = corners->bottomRight()) {
            drawCornerProperty(painter, rect, SubNodeIndex::BottomRight, borderSizes, bottomRight);
        }
    }
}

void drawLineProperty(QPainter *painter,
                      const QRect &rect,
                      SubNodeIndex subNodeIndex,
                      const QMarginsF &borderSizes,
                      const Union::Properties::LineProperty *line,
                      const Union::Properties::CornersProperty *corners)
{
    auto cornerRadii = corners ? corners->radii() : Union::Properties::CornersProperty::CornerRadii{};

    QRectF lineRect;
    switch (subNodeIndex) {
    case SubNodeIndex::Left: {
        const qreal topMargin = cornerRadii.topLeft;
        const qreal bottomMargin = cornerRadii.bottomLeft;

        lineRect = QRectF{static_cast<double>(rect.x()),
                          rect.top() + borderSizes.top() + topMargin,
                          line->size().value_or(0.0),
                          rect.height() - borderSizes.top() - borderSizes.bottom() - bottomMargin * 2};
    } break;
    case SubNodeIndex::Right: {
        const qreal topMargin = cornerRadii.topRight;
        const qreal bottomMargin = cornerRadii.bottomRight;

        lineRect = QRectF{rect.x() + rect.width() - line->size().value_or(0.0),
                          rect.top() + borderSizes.top() + topMargin,
                          line->size().value_or(0.0),
                          rect.height() - borderSizes.top() - borderSizes.bottom() - bottomMargin * 2};
    } break;
    case SubNodeIndex::Top: {
        const qreal leftMargin = cornerRadii.topLeft;
        const qreal rightMargin = cornerRadii.topRight;

        lineRect = QRectF{rect.x() + borderSizes.left() + leftMargin,
                          static_cast<double>(rect.y()),
                          rect.width() - borderSizes.left() - borderSizes.right() - rightMargin * 2,
                          line->size().value_or(0.0)};
    } break;
    case SubNodeIndex::Bottom: {
        const qreal leftMargin = cornerRadii.bottomLeft;
        const qreal rightMargin = cornerRadii.bottomRight;

        lineRect = QRectF{rect.x() + borderSizes.left() + leftMargin,
                          rect.height() - line->size().value_or(0.0) + rect.top(),
                          rect.width() - borderSizes.left() - borderSizes.right() - rightMargin * 2,
                          line->size().value_or(0.0)};
    } break;
    default:
        Q_UNREACHABLE();
    }

    if (auto image = line->image(); image && image->source()) {
        painter->drawImage(lineRect, imageCache.load(image->source().value(), lineRect.size()));
    }
    if (auto color = line->color()) {
        painter->setPen(Qt::transparent);
        painter->setBrush(color.value().toQColor());
        painter->drawRect(lineRect);

        // Draw corners *if* they do not have their own styled properties and have to be part of our border
        // Here's a bad but simple heuristic to detect that:
        const bool cornersHaveOwnStyling = corners->topLeft() && (corners->topLeft()->color().has_value() || corners->topLeft()->image());
        if (!cornersHaveOwnStyling) {
            const qreal lineWidth = line->size().value_or(0.0);
            if (lineWidth == 0.0) {
                return;
            }

            // NOTE: This assumes the corners are painted with the same border color
            painter->setPen(QPen(color.value().toQColor(), lineWidth));

            const auto lineMargin = QPointF{lineWidth, lineWidth} / 2.0;

            // Draw each of the two corners with their respective horizontal boundaries
            switch (subNodeIndex) {
            case SubNodeIndex::Top: {
                if (auto topLeft = corners->topLeft()) {
                    auto radius = topLeft->radius().value();
                    painter->drawArc(QRectF(rect.topLeft() + lineMargin, QSizeF(radius, radius) * 2), 90 * 16, 90 * 16);
                }
                if (auto topRight = corners->topRight()) {
                    auto radius = topRight->radius().value();
                    painter->drawArc(QRectF(rect.topRight() + lineMargin + QPointF(-radius, 0) * 2, QSizeF(radius, radius) * 2), 0 * 16, 90 * 16);
                }
            } break;
            case SubNodeIndex::Bottom: {
                if (auto bottomLeft = corners->bottomLeft()) {
                    auto radius = bottomLeft->radius().value();
                    painter->drawArc(QRectF(rect.bottomLeft() + lineMargin - QPointF(0, radius * 2), QSizeF(radius, radius) * 2), 180 * 16, 90 * 16);
                }
                if (auto bottomRight = corners->bottomRight()) {
                    auto radius = bottomRight->radius().value();
                    painter->drawArc(QRectF(rect.bottomRight() + lineMargin - QPointF(radius, radius) * 2, QSizeF(radius, radius) * 2), 270 * 16, 90 * 16);
                }
            }
            default:
                break;
            }
        }
    }
}

void drawCornerProperty(QPainter *painter,
                        const QRect &rect,
                        SubNodeIndex subNodeIndex,
                        const QMarginsF &borderSizes,
                        const Union::Properties::CornerProperty *corner)
{
    if (auto image = corner->image(); image && image->source()) {
        QRectF cornerRect;
        switch (subNodeIndex) {
        case SubNodeIndex::TopLeft: {
            const qreal width = corner->width().value_or(borderSizes.left());
            const qreal height = corner->height().value_or(borderSizes.top());
            cornerRect = QRectF{static_cast<double>(rect.left()), static_cast<double>(rect.top()), width, height};
        } break;
        case SubNodeIndex::TopRight: {
            const qreal width = corner->width().value_or(borderSizes.right());
            const qreal height = corner->height().value_or(borderSizes.top());
            cornerRect = QRectF{rect.left() + rect.width() - width, static_cast<double>(rect.top()), width, height};
        } break;
        case SubNodeIndex::BottomLeft: {
            const qreal width = corner->width().value_or(borderSizes.left());
            const qreal height = corner->height().value_or(borderSizes.bottom());
            cornerRect = QRectF{static_cast<double>(rect.left()), rect.height() + rect.top() - height, width, height};
        } break;
        case SubNodeIndex::BottomRight: {
            const qreal width = corner->width().value_or(borderSizes.right());
            const qreal height = corner->height().value_or(borderSizes.bottom());
            cornerRect = QRectF{rect.left() + rect.width() - width, rect.height() + rect.top() - height, width, height};
        } break;
        default:
            Q_UNREACHABLE();
        }

        painter->drawImage(cornerRect, imageCache.load(image->source().value(), cornerRect.size()));
    }
}
