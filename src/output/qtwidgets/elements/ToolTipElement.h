// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class ToolTipElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelTipLabel> : public TypeHelper<ToolTipElement, QStyleOptionFrame>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ToolTipLabelFrameWidth> : public TypeHelper<ToolTipElement, QStyleOptionFrame>{};
/* clang-format on */
}
}

class ToolTipElement : public AbstractElement
{
    Q_OBJECT

public:
    ToolTipElement(const QStyleOptionFrame *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~ToolTipElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    QStringList elementHints() const override;
    void layout() override;
    const QStyleOptionFrame *m_frameOption = nullptr;
};
