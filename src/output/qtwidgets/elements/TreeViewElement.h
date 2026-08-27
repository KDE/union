// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class TreeViewElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorBranch> : public TypeHelper<TreeViewElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_TreeViewIndentation> : public TypeHelper<TreeViewElement, QStyleOption>{};
/* clang-format on */
}
}

// This is a kitchen-sink element class to draw any various indicators

class TreeViewElement : public AbstractElement
{
    Q_OBJECT

public:
    TreeViewElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~TreeViewElement() override;

    void draw(QPainter *painter, DrawEnums enums) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    void drawIndicator(QPainter *painter) const override;
    const QStyleOption *m_treeViewOption = nullptr;
    qreal indentation() const;
};
