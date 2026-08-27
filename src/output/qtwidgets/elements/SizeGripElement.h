// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class SizeGripElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_SizeGrip> : public TypeHelper<SizeGripElement, QStyleOptionSizeGrip>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_SizeGrip> : public TypeHelper<SizeGripElement, QStyleOptionSizeGrip>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_SizeGripSize> : public TypeHelper<SizeGripElement, QStyleOptionSizeGrip>{};
/* clang-format on */
}
}

class SizeGripElement : public AbstractElement
{
    Q_OBJECT

public:
    SizeGripElement(const QStyleOptionSizeGrip *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~SizeGripElement() override;

    void update() override;
    void layout() override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;
    void draw(QPainter *painter, DrawEnums enums) const override;

private:
    const QStyleOptionSizeGrip *m_sizeGripOption = nullptr;
};
