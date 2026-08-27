// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "BackgroundDrawing.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

#include "ElementCache.h"

class UnionStyle;
class ProgressBarElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::SubElement,QStyle::SE_ProgressBarLabel> : public TypeHelper<ProgressBarElement, QStyleOptionProgressBar>{};
        template<> struct EnumToType<QStyle::SubElement,QStyle::SE_ProgressBarContents> : public TypeHelper<ProgressBarElement, QStyleOptionProgressBar>{};
        template<> struct EnumToType<QStyle::SubElement,QStyle::SE_ProgressBarGroove> : public TypeHelper<ProgressBarElement, QStyleOptionProgressBar>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ProgressBar> : public TypeHelper<ProgressBarElement, QStyleOptionProgressBar>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ProgressBarGroove> : public TypeHelper<ProgressBarElement, QStyleOptionProgressBar>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ProgressBarContents> : public TypeHelper<ProgressBarElement, QStyleOptionProgressBar>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ProgressBarLabel> : public TypeHelper<ProgressBarElement, QStyleOptionProgressBar>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_IndicatorProgressChunk> : public TypeHelper<ProgressBarElement, QStyleOptionProgressBar>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_ProgressBar> : public TypeHelper<ProgressBarElement, QStyleOptionProgressBar>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ProgressBarChunkWidth> : public TypeHelper<ProgressBarElement, QStyleOptionProgressBar>{};

/* clang-format on */
}
}

class ProgressBarElement : public AbstractElement
{
    Q_OBJECT

public:
    ProgressBarElement(const QStyleOptionProgressBar *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~ProgressBarElement() override;

    void update() override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subElementRect(QStyle::SubElement element) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    void updateSubElementList() override;
    void drawBackground(QPainter *painter) const override;
    void drawIndicator(QPainter *painter) const override;
    void drawChunk(QPainter *painter) const;
    const QStyleOptionProgressBar *m_progressBarOption = nullptr;
    int chunkWidth() const;
};
