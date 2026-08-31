// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "BackgroundDrawing.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

#include <ElementCache.h>

class UnionStyle;
class ToolButtonElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ComplexControl, QStyle::CC_ToolButton> : public TypeHelper<ToolButtonElement, QStyleOptionToolButton>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ToolButtonLabel> : public TypeHelper<ToolButtonElement, QStyleOptionToolButton>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameButtonTool> : public TypeHelper<ToolButtonElement, QStyleOptionToolButton>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelButtonTool> : public TypeHelper<ToolButtonElement, QStyleOptionToolButton>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_ToolButton> : public TypeHelper<ToolButtonElement, QStyleOptionToolButton>{};
/* clang-format on */
}
}

class ToolButtonElement : public AbstractElement
{
    Q_OBJECT

public:
    ToolButtonElement(const QStyleOptionToolButton *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~ToolButtonElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    void layout() override;

    QRectF subControlRect(QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;

private:
    QVariantMap elementAttributes() const override;
    QStringList elementHints() const override;
    void updateSubElementList() override;
    void drawIcon(QPainter *painter) const override;
    void drawIndicator(QPainter *painter) const override;
    void layoutButtons();

    const QStyleOptionToolButton *m_toolButtonOption = nullptr;
    bool m_hasIndicator;
    bool m_hasArrows;
    bool m_hasIcon;
    bool m_hasText;

    QRectF m_mainButtonRect;
    QRectF m_menuButtonRect;

    QMap<QString, LayoutItem> m_indicatorMap;
};
