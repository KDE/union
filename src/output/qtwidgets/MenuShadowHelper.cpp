/*
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2018, 2020 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 * SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// This implementation has been copied over from Breeze.
#include "MenuShadowHelper.h"

#include "BoxShadowRenderer.h"

#include "BackgroundDrawing.h"

#include <ElementQuery.h>
#include <QApplication>
#include <QDockWidget>
#include <QEvent>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QPlatformSurfaceEvent>
#include <QTextStream>
#include <QToolBar>

using namespace Union::Properties;
using namespace Qt::StringLiterals;

MenuShadowHelper::MenuShadowHelper()
    : QObject()
    , m_platform(Union::StyleRegistry::instance()->platform())
{
}

bool MenuShadowHelper::registerWidget(QWidget *widget, bool force)
{
    // make sure widget is not already registered
    if (m_registeredWidgets.contains(widget)) {
        return false;
    }

    // check if widget qualifies
    if (!(force || acceptWidget(widget))) {
        return false;
    }

    // try to create shadow directly
    installShadows(widget);
    m_registeredWidgets.insert(widget);

    widget->removeEventFilter(this);
    widget->installEventFilter(this);

    // connect destroy signal
    connect(widget, &QObject::destroyed, this, &MenuShadowHelper::widgetDeleted);

    return true;
}

void MenuShadowHelper::unregisterWidget(QWidget *widget)
{
    if (m_registeredWidgets.remove(widget)) {
        // uninstall the event filter
        widget->removeEventFilter(this);

        // disconnect all signals
        disconnect(widget, nullptr, this, nullptr);

        // uninstall the shadow
        uninstallShadows(widget);
    }
}

bool MenuShadowHelper::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() != QEvent::PlatformSurface) {
        return false;
    }

    QWidget *widget(static_cast<QWidget *>(object));
    QPlatformSurfaceEvent *surfaceEvent(static_cast<QPlatformSurfaceEvent *>(event));

    switch (surfaceEvent->surfaceEventType()) {
    case QPlatformSurfaceEvent::SurfaceCreated:
        installShadows(widget);
        break;
    case QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed:
        // Don't care.
        break;
    }

    return false;
}

QImage MenuShadowHelper::shadowTexture()
{
    Shadow params = lookupShadowProperties();
    if (params.isNone()) {
        return QImage();
    }
    const QSize boxSize = BoxShadowRenderer::calculateMinimumBoxSize(params);

    BoxShadowRenderer shadowRenderer;
    shadowRenderer.setBoxSize(boxSize);
    shadowRenderer.addShadow(params);

    QImage shadowTexture = shadowRenderer.render();

    const QRect outerRect(QPoint(0, 0), shadowTexture.size());

    QRect boxRect(QPoint(0, 0), boxSize);
    boxRect.moveCenter(outerRect.center());

    // Mask out inner rect, so when menu is turned off the shadow doesn't peek through.
    QPainter painter(&shadowTexture);
    painter.setRenderHint(QPainter::Antialiasing);

    const QMargins margins = QMargins(boxRect.left() - outerRect.left() - SHADOW_ADJUSTMENT - params.offset.x(),
                                      boxRect.top() - outerRect.top() - SHADOW_ADJUSTMENT - params.offset.y(),
                                      outerRect.right() - boxRect.right() - SHADOW_ADJUSTMENT + params.offset.x(),
                                      outerRect.bottom() - boxRect.bottom() - SHADOW_ADJUSTMENT + params.offset.y());

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    auto path = unevenRadiiRectPath(outerRect - margins, params.cornerRadius);
    painter.drawPath(path);

    painter.end();

    return shadowTexture;
}

void MenuShadowHelper::widgetDeleted(QObject *object)
{
    QWidget *widget(static_cast<QWidget *>(object));
    m_registeredWidgets.remove(widget);
}

bool MenuShadowHelper::acceptWidget(QWidget *widget) const
{
    if (qobject_cast<QMenu *>(widget)) {
        return true;
    }

    // combobox dropdown lists
    if (widget->inherits("QComboBoxPrivateContainer")) {
        return true;
    }

    if ((widget->inherits("QTipLabel") || (widget->windowFlags() & Qt::WindowType_Mask) == Qt::ToolTip) && !widget->inherits("Plasma::ToolTip")) {
        return true;
    }

    // detached widgets
    if (qobject_cast<QDockWidget *>(widget) || qobject_cast<QToolBar *>(widget)) {
        return true;
    }

    return false;
}

void MenuShadowHelper::installShadows(QWidget *widget)
{
    if (!widget) {
        return;
    }

    // only toplevel widgets can cast drop-shadows
    if (!widget->isWindow()) {
        return;
    }

    // widget must have valid native window
    if (!widget->testAttribute(Qt::WA_WState_Created)) {
        return;
    }

    const auto texture = shadowTexture();
    const auto margins = shadowMargins(widget);
    m_platform->installWindowShadow(widget, texture, margins);
}

QMargins MenuShadowHelper::shadowMargins(QWidget *widget) const
{
    Shadow params = lookupShadowProperties();
    const QSize boxSize = BoxShadowRenderer::calculateMinimumBoxSize(params);

    const QSizeF shadowSize = BoxShadowRenderer::calculateMinimumShadowTextureSize(boxSize, params);

    const QRectF shadowRect(QPoint(0, 0), shadowSize);

    QRectF boxRect(QPoint(0, 0), boxSize);
    boxRect.moveCenter(shadowRect.center());

    QMarginsF margins(boxRect.left() - shadowRect.left() - SHADOW_ADJUSTMENT - params.offset.x(),
                      boxRect.top() - shadowRect.top() - SHADOW_ADJUSTMENT - params.offset.y(),
                      shadowRect.right() - boxRect.right() - SHADOW_ADJUSTMENT + params.offset.x(),
                      shadowRect.bottom() - boxRect.bottom() - SHADOW_ADJUSTMENT + params.offset.y());

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

void MenuShadowHelper::uninstallShadows(QWidget *widget)
{
    m_platform->uninstallWindowShadow(widget->windowHandle());
}

Shadow MenuShadowHelper::lookupShadowProperties()
{
    Union::ElementList elements;
    QStringList elementTypes = {u"menu"_s, u"popup"_s};
    for (const auto &elementType : elementTypes) {
        auto unionElement = Union::Element::create();
        unionElement->setType(elementType);
        elements.append(unionElement);
    }

    Q_ASSERT(!elements.isEmpty());
    const auto style = Union::StyleRegistry::instance()->defaultStyle();
    const auto query = std::make_unique<Union::ElementQuery>(style);
    query->setElements(elements);
    query->execute();
    const auto properties = query->properties();

    const auto shadowColor = safePropertyLookup(properties, Union::Color(), &StylePropertyGroup::shadow, &ShadowPropertyGroup::color);
    const auto shadowBlur = safePropertyLookup(properties, 0.0, &StylePropertyGroup::shadow, &ShadowPropertyGroup::blur);
    const auto shadowSize = safePropertyLookup(properties, 0.0, &StylePropertyGroup::shadow, &ShadowPropertyGroup::size);
    const auto shadowRadius = safePropertyLookup(properties, CornersPropertyGroup::CornerRadii(), &StylePropertyGroup::corners, &CornersPropertyGroup::radii);
    const auto shadowOffset =
        safePropertyLookup(properties, QVector2D(), &StylePropertyGroup::shadow, &ShadowPropertyGroup::offset, &OffsetPropertyGroup::toVector2D);

    // We need to portrude shadow around the edges for the blurAmount (or shadowSize??)
    auto params = Shadow(QPoint(shadowOffset.x(), shadowOffset.y()), shadowBlur, shadowSize, shadowColor.toQColor());
    params.cornerRadius = shadowRadius;
    return params;
}
