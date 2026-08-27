// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "BackgroundDrawing.h"
#include <ElementCache.h>
#include <QIcon>
#include <QObject>
#include <QStyleOption>

class UnionStyle;
class ToolBoxTabElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::SubElement, QStyle::SE_ToolBoxTabContents> : public TypeHelper<ToolBoxTabElement, QStyleOptionToolBox>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ToolBoxTabShape> : public TypeHelper<ToolBoxTabElement, QStyleOptionToolBox>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ToolBoxTabLabel> : public TypeHelper<ToolBoxTabElement, QStyleOptionToolBox>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ToolBoxTab> : public TypeHelper<ToolBoxTabElement, QStyleOptionToolBox>{};
/* clang-format on */
}
}

class ToolBoxTabElement : public AbstractElement
{
    Q_OBJECT

public:
    ToolBoxTabElement(const QStyleOptionToolBox *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~ToolBoxTabElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subElementRect(QStyle::SubElement element) const override;

private:
    void updateSubElementList() override;
    const QStyleOptionToolBox *m_toolBoxOption = nullptr;
};
