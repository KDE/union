// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class WidgetElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_Widget> : public TypeHelper<WidgetElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::StyleHint, QStyle::SH_FormLayoutWrapPolicy> : public TypeHelper<WidgetElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::StyleHint, QStyle::SH_FormLayoutFieldGrowthPolicy> : public TypeHelper<WidgetElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::StyleHint, QStyle::SH_FormLayoutFormAlignment> : public TypeHelper<WidgetElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::StyleHint, QStyle::SH_FormLayoutLabelAlignment> : public TypeHelper<WidgetElement, QStyleOption>{};
/* clang-format on */
}
}

class WidgetElement : public AbstractElement
{
    Q_OBJECT

public:
    WidgetElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~WidgetElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    int styleHint(QStyle::StyleHint styleHint) const override;

private:
    const QStyleOption *m_widgetOption = nullptr;

    void drawKMultiTabBar(QPainter *painter) const;
};
