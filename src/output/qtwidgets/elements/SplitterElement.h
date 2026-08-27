// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class SplitterElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_Splitter> : public TypeHelper<SplitterElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_Splitter> : public TypeHelper<SplitterElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_SplitterWidth> : public TypeHelper<SplitterElement, QStyleOption>{};
/* clang-format on */
}
}

class SplitterElement : public AbstractElement
{
    Q_OBJECT

public:
    SplitterElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~SplitterElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    const QStyleOption *m_splitterOption = nullptr;
};
