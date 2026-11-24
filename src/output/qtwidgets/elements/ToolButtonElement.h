// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

enum class ArrowStyle {
    None,
    Menu,
    InlineArrow,
    MenuButton,
};

class ToolButtonElement : public AbstractElement
{
    Q_OBJECT

public:
    ToolButtonElement(const UnionStyle *style, QWidget *widget = nullptr);
    ~ToolButtonElement() override;

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;

    QRectF subControlRect(const QStyleOption *option, QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const override;

private:
    // void drawIcon(QPainter *painter) const override;
    // void drawIndicator(QPainter *painter) const override;
    // void layoutButtons();
    ArrowStyle arrowStyle() const;
    //
    // const QStyleOptionToolButton *m_toolButtonOption = nullptr;
    // bool m_hasIndicator;
    // bool m_hasArrows;
    // bool m_hasIcon;
    // bool m_hasText;
    ArrowStyle m_arrowStyle = ArrowStyle::None;
    Qt::ToolButtonStyle m_toolButtonStyle = Qt::ToolButtonFollowStyle;
    //
    // QRectF m_mainButtonRect;
    // QRectF m_menuButtonRect;
    //
};
}
}
