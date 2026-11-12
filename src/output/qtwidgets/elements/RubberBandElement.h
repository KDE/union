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
class RubberBandElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_RubberBand> : public TypeHelper<RubberBandElement, QStyleOption>{};
/* clang-format on */
}
}

class RubberBandElement : public AbstractElement
{
    Q_OBJECT

public:
    RubberBandElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~RubberBandElement() override;

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

private:
    QString fallbackElementName(const QStyleOption *opt) const override;
    const QStyleOptionRubberBand *m_rubberBandOption = nullptr;
};
}
}
