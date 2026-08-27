// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "BackgroundDrawing.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>
#include <qstyleoption.h>

#include <ElementCache.h>

class UnionStyle;
class DockWidgetElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::SubElement, QStyle::SE_DockWidgetTitleBarText> : public TypeHelper<DockWidgetElement, QStyleOptionDockWidget>{};
        template<> struct EnumToType<QStyle::SubElement, QStyle::SE_DockWidgetCloseButton> : public TypeHelper<DockWidgetElement, QStyleOptionDockWidget>{};
        template<> struct EnumToType<QStyle::SubElement, QStyle::SE_DockWidgetFloatButton> : public TypeHelper<DockWidgetElement, QStyleOptionDockWidget>{};
        template<> struct EnumToType<QStyle::SubElement, QStyle::SE_DockWidgetIcon> : public TypeHelper<DockWidgetElement, QStyleOptionDockWidget>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_DockWidgetTitle> : public TypeHelper<DockWidgetElement, QStyleOptionDockWidget>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_DockWidgetFrameWidth> : public TypeHelper<DockWidgetElement, QStyleOptionDockWidget>{};
/* clang-format on */
}
}

class DockWidgetElement : public AbstractElement
{
    Q_OBJECT

public:
    DockWidgetElement(const QStyleOptionDockWidget *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~DockWidgetElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subElementRect(QStyle::SubElement subElement) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    void updateSubElementList() override;
    const QStyleOptionDockWidget *m_dockWidgetOption = nullptr;
};
