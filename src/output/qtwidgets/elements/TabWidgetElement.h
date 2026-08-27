// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class TabWidgetElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameTabWidget> : public TypeHelper<TabWidgetElement, QStyleOptionTabWidgetFrame>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_TabWidget> : public TypeHelper<TabWidgetElement, QStyleOptionTabWidgetFrame>{};
/* clang-format on */
}
}

class TabWidgetElement : public AbstractElement
{
    Q_OBJECT

public:
    TabWidgetElement(const QStyleOptionTabWidgetFrame *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~TabWidgetElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

private:
    QVariantMap elementAttributes() const override;
    QStringList elementHints() const override;
    const QStyleOptionTabWidgetFrame *m_tabFrameOption = nullptr;
};
