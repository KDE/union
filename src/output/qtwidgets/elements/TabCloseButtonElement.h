// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class TabCloseButtonElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorTabClose> : public TypeHelper<TabCloseButtonElement, QStyleOption>{};
/* clang-format on */
}
}

class TabCloseButtonElement : public AbstractElement
{
    Q_OBJECT

public:
    TabCloseButtonElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~TabCloseButtonElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

private:
    const QStyleOption *m_widgetOption = nullptr;
};
