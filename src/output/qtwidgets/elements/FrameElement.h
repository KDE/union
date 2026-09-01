// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class FrameElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_FocusFrame> : public TypeHelper<FrameElement, QStyleOptionFrame>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ShapedFrame> : public TypeHelper<FrameElement, QStyleOptionFrame>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameDockWidget> : public TypeHelper<FrameElement, QStyleOptionFrame>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameWindow> : public TypeHelper<FrameElement, QStyleOptionFrame>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_Frame> : public TypeHelper<FrameElement, QStyleOptionFrame>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_DefaultFrameWidth> : public TypeHelper<FrameElement, QStyleOptionFrame>{};
/* clang-format on */
}
}

class FrameElement : public AbstractElement
{
    Q_OBJECT

public:
    FrameElement(const QStyleOptionFrame *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~FrameElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    void drawFrame(QPainter *painter) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    QVariantMap elementAttributes() const override;
    QStringList elementHints() const override;
    void updateSubElementList() override;
    const QStyleOptionFrame *m_frameOption = nullptr;
};
