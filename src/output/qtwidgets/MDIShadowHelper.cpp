/*
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

// This implementation has been copied over from Breeze.

#include "MDIShadowHelper.h"

#include "BoxShadowRenderer.h"
#include "MenuShadowHelper.h"

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPainter>
#include <QTextStream>

MDIWindowShadow::MDIWindowShadow(QWidget *parent, const QImage &shadowImage)
    : QWidget(parent)
    , m_shadowImage(shadowImage)
    , m_platform(Union::StyleRegistry::instance()->platform())
{
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFocusPolicy(Qt::NoFocus);
}

void MDIWindowShadow::updateGeometry()
{
    if (!m_widget) {
        return;
    }

    const auto params = MenuShadowHelper::lookupShadowProperties();
    if (params.isNone()) {
        return;
    }

    const QSizeF boxSize = BoxShadowRenderer::calculateMinimumBoxSize(params);
    const QSizeF shadowSize = BoxShadowRenderer::calculateMinimumShadowTextureSize(boxSize, params);
    const QRectF shadowRect(QPoint(0, 0), shadowSize);

    QRectF boxRect(QPoint(0, 0), boxSize);
    boxRect.moveCenter(shadowRect.center());

    const double topSize(boxRect.top() - shadowRect.top() - SHADOW_ADJUSTMENT - params.offset.y());
    const double bottomSize(shadowRect.bottom() - boxRect.bottom() - SHADOW_ADJUSTMENT + params.offset.y());
    const double leftSize(boxRect.left() - shadowRect.left() - SHADOW_ADJUSTMENT - params.offset.x());
    const double rightSize(shadowRect.right() - boxRect.right() - SHADOW_ADJUSTMENT + params.offset.x());

    auto hole = m_widget->frameGeometry();
    m_shadowTilesRect = hole.adjusted(-leftSize, -topSize, rightSize, bottomSize);

    auto parent(parentWidget());
    if (parent && !qobject_cast<QMdiArea *>(parent) && qobject_cast<QMdiArea *>(parent->parentWidget())) {
        parent = parent->parentWidget();
    }

    if (qobject_cast<QAbstractScrollArea *>(parent)) {
        parent = qobject_cast<QAbstractScrollArea *>(parent)->viewport();
    }

    // update geometry and mask
    QRect geometry(m_shadowTilesRect);
    if (parent) {
        geometry &= parent->rect();
        hole &= parent->rect();
    }
    const QRegion mask = QRegion(geometry) - hole.adjusted(SHADOW_ADJUSTMENT, SHADOW_ADJUSTMENT, -SHADOW_ADJUSTMENT, -SHADOW_ADJUSTMENT);
    if (mask.isEmpty()) {
        hide();
    } else {
        setGeometry(geometry);
        setMask(mask.translated(-geometry.topLeft()));
        show();
    }

    m_shadowTilesRect.translate(-geometry.topLeft());
}

void MDIWindowShadow::updateZOrder()
{
    stackUnder(m_widget);
}

void MDIWindowShadow::paintEvent(QPaintEvent *event)
{
    if (m_shadowImage.isNull()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setClipRegion(event->region());
    Union::StyleRegistry::instance()->platform()->drawMdiShadow(&painter, m_shadowTilesRect);
}

void MDIWindowShadow::setWidget(QWidget *value)
{
    m_widget = value;
    m_platform->installWindowShadow(m_widget, m_shadowImage, QMargins());
}

QWidget *MDIWindowShadow::widget() const
{
    return m_widget;
}

MDIShadowHelper::MDIShadowHelper()
    : QObject()
{
}

bool MDIShadowHelper::registerWidget(QWidget *widget)
{
    auto subwindow(qobject_cast<QMdiSubWindow *>(widget));
    if (!subwindow) {
        return false;
    }
    if (subwindow->widget() && subwindow->widget()->inherits("KMainWindow")) {
        return false;
    }

    // make sure widget is not already registered
    if (m_registeredWidgets.contains(widget)) {
        return false;
    }

    m_registeredWidgets.insert(widget);
    // create shadow immediately if widget is already visible
    if (widget->isVisible()) {
        installShadow(widget);
        updateShadowGeometry(widget);
        updateShadowZOrder(widget);
    }

    widget->installEventFilter(this);

    connect(widget, &QObject::destroyed, this, &MDIShadowHelper::widgetDestroyed);

    return true;
}

void MDIShadowHelper::setShadowHelper(MenuShadowHelper *shadowHelper)
{
    m_shadowHelper = shadowHelper;
}

void MDIShadowHelper::unregisterWidget(QWidget *widget)
{
    if (!m_registeredWidgets.contains(widget)) {
        return;
    }
    widget->removeEventFilter(this);
    m_registeredWidgets.remove(widget);
    removeShadow(widget);
}

bool MDIShadowHelper::eventFilter(QObject *object, QEvent *event)
{
    switch (event->type()) {
    case QEvent::ZOrderChange:
        updateShadowZOrder(object);
        break;

    case QEvent::Hide:
        hideShadows(object);
        break;

    case QEvent::Show:
        installShadow(object);
        updateShadowGeometry(object);
        updateShadowZOrder(object);
        break;

    case QEvent::Move:
    case QEvent::Resize:
        updateShadowGeometry(object);
        break;

    default:
        break;
    }

    return QObject::eventFilter(object, event);
}

MDIWindowShadow *MDIShadowHelper::findShadow(QObject *object) const
{
    if (object && !object->parent()) {
        return nullptr;
    }

    // find existing window shadows
    const auto children = object->parent()->children();
    for (QObject *child : children) {
        if (MDIWindowShadow *shadow = qobject_cast<MDIWindowShadow *>(child)) {
            if (shadow->widget() == object) {
                return shadow;
            }
        }
    }

    return nullptr;
}

void MDIShadowHelper::installShadow(QObject *object)
{
    auto widget(static_cast<QWidget *>(object));
    if (!widget->parentWidget()) {
        return;
    }

    // make sure shadow is not already installed
    if (findShadow(object)) {
        return;
    }

    if (!m_shadowHelper) {
        return;
    }

    // create new shadow
    auto windowShadow(new MDIWindowShadow(widget->parentWidget(), m_shadowHelper->shadowTexture()));
    windowShadow->setWidget(widget);
}

void MDIShadowHelper::removeShadow(QObject *object)
{
    if (MDIWindowShadow *windowShadow = findShadow(object)) {
        windowShadow->hide();
        windowShadow->deleteLater();
    }
}

void MDIShadowHelper::widgetDestroyed(QObject *object)
{
    m_registeredWidgets.remove(object);
    removeShadow(object);
}

void MDIShadowHelper::hideShadows(QObject *object) const
{
    if (MDIWindowShadow *windowShadow = findShadow(object)) {
        windowShadow->hide();
    }
}

void MDIShadowHelper::updateShadowZOrder(QObject *object) const
{
    if (MDIWindowShadow *windowShadow = findShadow(object)) {
        if (!windowShadow->isVisible()) {
            windowShadow->show();
        }
        windowShadow->updateZOrder();
    }
}

void MDIShadowHelper::updateShadowGeometry(QObject *object) const
{
    if (MDIWindowShadow *windowShadow = findShadow(object)) {
        windowShadow->updateGeometry();
    }
}

void MDIShadowHelper::updateShadows(QObject *object) const
{
    if (MDIWindowShadow *windowShadow = findShadow(object)) {
        windowShadow->update();
    }
}
