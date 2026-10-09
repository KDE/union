// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "BoxShadowRenderer.h"
#include "StyleRegistry.h"

#include "properties/StylePropertyGroup.h"
#include <QColor>
#include <QMap>
#include <QMargins>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QWindow>

class ShadowRenderer : public QObject
{
    Q_OBJECT
public:
    explicit ShadowRenderer(const Union::Properties::StylePropertyGroup *properties);

    BoxShadowRenderer::Shadow shadowProperties() const;

    void drawShadow(QWidget *widget);

    QImage shadowTexture();

protected Q_SLOTS:

    void shadowRemoved(QObject *object);

protected:
    bool isRenderAllowed(QWidget *widget) const;

    QMargins margins(QWidget *widget) const;

private:
    std::shared_ptr<Union::PlatformPlugin> m_platform;

    const Union::Properties::StylePropertyGroup *m_properties;
};
