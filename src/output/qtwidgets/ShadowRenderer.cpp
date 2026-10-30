// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ShadowRenderer.h"

#include "BackgroundDrawing.h"

#include <ElementQuery.h>
#include <QApplication>
#include <QDockWidget>
#include <QEvent>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QPlatformSurfaceEvent>
#include <QTextStream>
#include <QToolBar>

using namespace Union::Properties;
using namespace Qt::StringLiterals;

ShadowRenderer::ShadowRenderer(const Union::StylePropertyGroupPtr &properties)
    : QObject()
    , m_platform(Union::StyleRegistry::instance()->platform())
    , m_properties(properties)
{
}

void ShadowRenderer::drawShadow(QWidget *widget)
{
    if (!isRenderAllowed(widget)) {
        return;
    }

    if (widget) {
        if (const auto window = widget->windowHandle()) {
            m_platform->installWindowShadow(window, shadowTexture(), margins(widget));
        }
        connect(widget, &QObject::destroyed, this, &ShadowRenderer::shadowRemoved);
    }
}

QImage ShadowRenderer::shadowTexture()
{
    BoxShadowRenderer::Shadow params = shadowProperties();
    if (params.isNone()) {
        return QImage();
    }
    const QSize boxSize = BoxShadowRenderer::calculateMinimumBoxSize(params);

    BoxShadowRenderer boxShadowRenderer;
    boxShadowRenderer.setBoxSize(boxSize);
    boxShadowRenderer.addShadow(params);

    QImage shadowTexture = boxShadowRenderer.render();

    const QRect outerRect(QPoint(0, 0), shadowTexture.size());

    QRect boxRect(QPoint(0, 0), boxSize);
    boxRect.moveCenter(outerRect.center());

    // Mask out inner rect, so when menu is turned off the shadow doesn't peek through.
    QPainter painter(&shadowTexture);
    painter.setRenderHint(QPainter::Antialiasing);

    const QMargins margins = QMargins(boxRect.left() - outerRect.left() - BoxShadowRenderer::SHADOW_ADJUSTMENT - params.offset.x(),
                                      boxRect.top() - outerRect.top() - BoxShadowRenderer::SHADOW_ADJUSTMENT - params.offset.y(),
                                      outerRect.right() - boxRect.right() - BoxShadowRenderer::SHADOW_ADJUSTMENT + params.offset.x(),
                                      outerRect.bottom() - boxRect.bottom() - BoxShadowRenderer::SHADOW_ADJUSTMENT + params.offset.y());

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    auto path = unevenRadiiRectPath(outerRect - margins, params.cornerRadius);
    painter.drawPath(path);

    painter.end();

    return shadowTexture;
}

void ShadowRenderer::shadowRemoved(QObject *object)
{
    if (const auto widget = static_cast<QWidget *>(object)) {
        disconnect(widget, &QObject::destroyed, this, &ShadowRenderer::shadowRemoved);
        if (const auto window = widget->windowHandle()) {
            m_platform->uninstallWindowShadow(window);
        }
    }
}

bool ShadowRenderer::isRenderAllowed(QWidget *widget) const
{
    // Test if widget is a valid native window
    if (!widget || !widget->testAttribute(Qt::WA_WState_Created)) {
        return false;
    } else if (qobject_cast<QMenu *>(widget)) {
        return true;
    } else if (widget->inherits("QComboBoxPrivateContainer")) {
        return true;
    } else if ((widget->inherits("QTipLabel") || (widget->windowFlags() & Qt::WindowType_Mask) == Qt::ToolTip) && !widget->inherits("Plasma::ToolTip")) {
        // Skip plasma tooltips
        return true;
    } else if (qobject_cast<QDockWidget *>(widget) || qobject_cast<QToolBar *>(widget)) {
        return true;
    }

    return false;
}

QMargins ShadowRenderer::margins(QWidget *widget) const
{
    BoxShadowRenderer::Shadow params = shadowProperties();
    const QSize boxSize = BoxShadowRenderer::calculateMinimumBoxSize(params);

    const QSizeF shadowSize = BoxShadowRenderer::calculateMinimumShadowTextureSize(boxSize, params);

    const QRectF shadowRect(QPoint(0, 0), shadowSize);

    QRectF boxRect(QPoint(0, 0), boxSize);
    boxRect.moveCenter(shadowRect.center());

    QMarginsF margins(boxRect.left() - shadowRect.left() - BoxShadowRenderer::SHADOW_ADJUSTMENT - params.offset.x(),
                      boxRect.top() - shadowRect.top() - BoxShadowRenderer::SHADOW_ADJUSTMENT - params.offset.y(),
                      shadowRect.right() - boxRect.right() - BoxShadowRenderer::SHADOW_ADJUSTMENT + params.offset.x(),
                      shadowRect.bottom() - boxRect.bottom() - BoxShadowRenderer::SHADOW_ADJUSTMENT + params.offset.y());

    // Balloon tip needs special margins to deal with the arrow.
    if (widget->inherits("QBalloonTip")) {
        int top = widget->contentsMargins().top();
        int bottom = widget->contentsMargins().bottom();

        // Need to decrement default size further due to extra hard coded round corner.
        margins -= 1;

        // Arrow can be either to the top or the bottom. Adjust margins accordingly.
        const int diff = qAbs(top - bottom);
        if (top > bottom) {
            margins.setTop(margins.top() - diff);
        } else {
            margins.setBottom(margins.bottom() - diff);
        }
    }

    return margins.toMargins();
}

BoxShadowRenderer::Shadow ShadowRenderer::shadowProperties() const
{
    BoxShadowRenderer::Shadow params;
    if (m_properties) {
        const auto shadowColor = safePropertyLookup(m_properties, Union::Color(), &StylePropertyGroup::shadow, &ShadowPropertyGroup::color);
        const auto shadowBlur = safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::shadow, &ShadowPropertyGroup::blur);
        const auto shadowSize = safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::shadow, &ShadowPropertyGroup::size);
        const auto shadowRadius =
            safePropertyLookup(m_properties, CornersPropertyGroup::CornerRadii(), &StylePropertyGroup::corners, &CornersPropertyGroup::radii);
        const auto shadowOffset =
            safePropertyLookup(m_properties, QVector2D(), &StylePropertyGroup::shadow, &ShadowPropertyGroup::offset, &OffsetPropertyGroup::toVector2D);

        // We need to portrude shadow around the edges for the blurAmount (or shadowSize??)
        params = BoxShadowRenderer::Shadow(QPoint(shadowOffset.x(), shadowOffset.y()), shadowBlur, shadowSize, shadowColor.toQColor());
        params.cornerRadius = shadowRadius;
    }
    return params;
}
