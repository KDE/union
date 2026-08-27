// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "BackgroundDrawing.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

#include "../ElementCache.h"

class UnionStyle;

class ItemViewElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_ItemViewItemText> : public TypeHelper<ItemViewElement, QStyleOptionViewItem>{};
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_ItemViewItemDecoration> : public TypeHelper<ItemViewElement, QStyleOptionViewItem>{};
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_ItemViewItemCheckIndicator> : public TypeHelper<ItemViewElement, QStyleOptionViewItem>{};
    template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ItemViewItem> : public TypeHelper<ItemViewElement, QStyleOptionViewItem>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelItemViewItem> : public TypeHelper<ItemViewElement, QStyleOptionViewItem>{};
    template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_ItemViewItem> : public TypeHelper<ItemViewElement, QStyleOptionViewItem>{};
/* clang-format on */
}
}

class ItemViewElement : public AbstractElement
{
    Q_OBJECT

public:
    ItemViewElement(const QStyleOptionViewItem *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~ItemViewElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subElementRect(QStyle::SubElement element) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;

private:
    QVariantMap elementAttributes() const override;
    QStringList elementHints() const override;
    void updateSubElementList() override;
    void drawIndicator(QPainter *painter) const override;
    void drawText(QPainter *painter) const override;
    void drawIcon(QPainter *painter) const override;
    const QStyleOptionViewItem *m_viewItemOption = nullptr;
};
