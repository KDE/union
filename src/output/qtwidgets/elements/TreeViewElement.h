// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "ElementCache.h"
#include <QObject>
#include <QStyleOption>

namespace Union
{
namespace Widgets
{

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

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    Union::IdentifierList elementHints() const override;
    void drawIndicator(QPainter *painter) const override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    qreal indentation() const;
    bool m_itemIsOpen;

    Union::ElementList m_indicatorElementList;
    Union::StylePropertyGroupPtr m_indicatorProperties = nullptr;
};

}
}
