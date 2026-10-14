/*
 * SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
 * SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "StyleRegistry.h"

#include <QEvent>
#include <QObject>
#include <QSet>

#include <QPaintEvent>
#include <QPointer>
#include <QWidget>

class MenuShadowHelper;

//* frame shadow
/** this allows the shadow to be painted over the widgets viewport */
class MDIWindowShadow : public QWidget
{
    Q_OBJECT

public:
    explicit MDIWindowShadow(QWidget *parent, const QImage &shadowImage);

    void updateGeometry();

    void updateZOrder();

    void setWidget(QWidget *value);

    QWidget *widget() const;

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QWidget *m_widget = nullptr;

    //* tileset rect, used for painting
    QRect m_shadowTilesRect;

    QImage m_shadowImage;

    std::shared_ptr<Union::PlatformPlugin> m_platform;
};

//* shadow manager
class MDIShadowHelper : public QObject
{
    Q_OBJECT

public:
    explicit MDIShadowHelper();

    void setShadowHelper(MenuShadowHelper *shadowHelper);

    bool registerWidget(QWidget *);
    void unregisterWidget(QWidget *);

    bool eventFilter(QObject *, QEvent *) override;

protected:
    MDIWindowShadow *findShadow(QObject *) const;

    void installShadow(QObject *);

    void removeShadow(QObject *);

    void hideShadows(QObject *object) const;

    void updateShadowZOrder(QObject *object) const;

    void updateShadowGeometry(QObject *object) const;

    void updateShadows(QObject *object) const;

protected Q_SLOTS:
    void widgetDestroyed(QObject *);

private:
    //* set of registered widgets
    QSet<const QObject *> m_registeredWidgets;

    //* shadow helper used to generate the shadows
    QPointer<MenuShadowHelper> m_shadowHelper;
};
