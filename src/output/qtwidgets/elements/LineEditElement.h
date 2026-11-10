// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "ElementCache.h"
#include <QObject>
#include <QStyleOption>

class UnionStyle;

class LineEditElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
    template<> struct EnumToType<QStyle::SubElement, QStyle::SE_LineEditContents> : public TypeHelper<LineEditElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameLineEdit> : public TypeHelper<LineEditElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_PanelLineEdit> : public TypeHelper<LineEditElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_LineEdit> : public TypeHelper<LineEditElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_LineEditIconMargin> : public TypeHelper<LineEditElement, QStyleOption>{};
    template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_LineEditIconSize> : public TypeHelper<LineEditElement, QStyleOption>{};
/* clang-format on */
}
}

class LineEditElement : public AbstractElement
{
    Q_OBJECT

public:
    LineEditElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~LineEditElement() override;

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    void drawBackground(QPainter *painter) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;

    QRectF subElementRect(QStyle::SubElement element) const override;

private:
    void updateSubElementList() override;
    Union::IdentifierVariantHash elementAttributes() const override;
    Union::IdentifierList elementHints() const override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    QMarginsF iconPadding() const;
    const QStyleOptionFrame *m_frameOption = nullptr;
    bool m_hasFrame;
};
