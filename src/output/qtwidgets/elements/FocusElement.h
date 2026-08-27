// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class FocusElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameFocusRect> : public TypeHelper<FocusElement, QStyleOptionFocusRect>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_FocusFrameVMargin> : public TypeHelper<FocusElement, QStyleOptionFocusRect>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_FocusFrameHMargin> : public TypeHelper<FocusElement, QStyleOptionFocusRect>{};
/* clang-format on */
}
}

class FocusElement : public AbstractElement
{
    Q_OBJECT

public:
    FocusElement(const QStyleOptionFocusRect *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~FocusElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    QStringList elementHints() const override;
    const QStyleOptionFocusRect *m_focusOption = nullptr;
};
