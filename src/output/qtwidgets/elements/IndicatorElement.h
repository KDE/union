// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class IndicatorElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorArrowLeft> : public TypeHelper<IndicatorElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorArrowUp> : public TypeHelper<IndicatorElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorArrowRight> : public TypeHelper<IndicatorElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorArrowDown> : public TypeHelper<IndicatorElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorButtonDropDown> : public TypeHelper<IndicatorElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ListViewIconSize> : public TypeHelper<IndicatorElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_SmallIconSize> : public TypeHelper<IndicatorElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_IconViewIconSize> : public TypeHelper<IndicatorElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_LargeIconSize> : public TypeHelper<IndicatorElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_MessageBoxIconSize> : public TypeHelper<IndicatorElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_TextCursorWidth> : public TypeHelper<IndicatorElement, QStyleOption>{};
/* clang-format on */
}
}

// This is a kitchen-sink element class to draw any various indicators

class IndicatorElement : public AbstractElement
{
    Q_OBJECT

public:
    IndicatorElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~IndicatorElement() override;

    void draw(QPainter *painter, DrawEnums enums) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    const QStyleOption *m_indicatorOption = nullptr;

    void drawElement(QPainter *painter, const QString &defaultIconName, QStringList targetHierarchy) const;
};
