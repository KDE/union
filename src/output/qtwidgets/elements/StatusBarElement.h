// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "BackgroundDrawing.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class StatusBarElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelStatusBar> : public TypeHelper<StatusBarElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameStatusBarItem> : public TypeHelper<StatusBarElement, QStyleOption>{};
/* clang-format on */
}
}

class StatusBarElement : public AbstractElement
{
    Q_OBJECT

public:
    StatusBarElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~StatusBarElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

private:
    void drawItem(QPainter *painter) const;
    const QStyleOption *m_statusBarOption = nullptr;
    void updateSubElementList() override;
};
