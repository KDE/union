// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class TabBarElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameTabBarBase> : public TypeHelper<TabBarElement, QStyleOptionTabBarBase>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_TabBarScrollButtonWidth> : public TypeHelper<TabBarElement, QStyleOptionTabBarBase>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_TabBarBaseHeight> : public TypeHelper<TabBarElement, QStyleOptionTabBarBase>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_TabBarIconSize> : public TypeHelper<TabBarElement, QStyleOptionTabBarBase>{};
/* clang-format on */
}
}

class TabBarElement : public AbstractElement
{
    Q_OBJECT

public:
    TabBarElement(const QStyleOptionTabBarBase *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~TabBarElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    QVariantMap elementAttributes() const override;
    void drawBackground(QPainter *painter) const override;
    const QStyleOptionTabBarBase *m_tabBarOption = nullptr;
    qreal scrollButtonWidth() const;
};
