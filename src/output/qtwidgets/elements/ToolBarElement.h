// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "BackgroundDrawing.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class ToolBarElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ToolBar> : public TypeHelper<ToolBarElement, QStyleOptionToolBar>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorToolBarHandle> : public TypeHelper<ToolBarElement, QStyleOptionToolBar>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorToolBarSeparator> : public TypeHelper<ToolBarElement, QStyleOptionToolBar>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelToolBar> : public TypeHelper<ToolBarElement, QStyleOptionToolBar>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ToolBarSeparatorExtent> : public TypeHelper<ToolBarElement, QStyleOptionToolBar>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ToolBarFrameWidth> : public TypeHelper<ToolBarElement, QStyleOptionToolBar>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ToolBarHandleExtent> : public TypeHelper<ToolBarElement, QStyleOptionToolBar>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ToolBarExtensionExtent> : public TypeHelper<ToolBarElement, QStyleOptionToolBar>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ToolBarItemSpacing> : public TypeHelper<ToolBarElement, QStyleOptionToolBar>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ToolBarItemMargin> : public TypeHelper<ToolBarElement, QStyleOptionToolBar>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ToolBarIconSize> : public TypeHelper<ToolBarElement, QStyleOptionToolBar>{};
/* clang-format on */
}
}

class ToolBarElement : public AbstractElement
{
    Q_OBJECT

public:
    ToolBarElement(const QStyleOptionToolBar *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~ToolBarElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    void updateSubElementList() override;
    void drawHandle(QPainter *painter) const;
    void drawSeparator(QPainter *painter) const;
    const QStyleOptionToolBar *m_toolBarOption = nullptr;

    Union::ElementList m_handleElementList;
    Union::Properties::StylePropertyGroup *m_handleProperties;
    Union::ElementList m_separatorElementList;
    Union::Properties::StylePropertyGroup *m_separatorProperties;
    Union::ElementList m_extensionElementList;
    Union::Properties::StylePropertyGroup *m_extensionProperties;

    qreal separatorExtent() const;
    qreal handleExtent() const;
    qreal extensionExtent() const;
};
