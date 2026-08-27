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
class ComboBoxElement;
namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ComplexControl, QStyle::CC_ComboBox> : public TypeHelper<ComboBoxElement, QStyleOptionComboBox>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ComboBoxLabel> : public TypeHelper<ComboBoxElement, QStyleOptionComboBox>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_ComboBox> : public TypeHelper<ComboBoxElement, QStyleOptionComboBox>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ComboBoxFrameWidth> : public TypeHelper<ComboBoxElement, QStyleOptionComboBox>{};
/* clang-format on */
}
}

class ComboBoxElement : public AbstractElement
{
    Q_OBJECT

public:
    ComboBoxElement(const QStyleOptionComboBox *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~ComboBoxElement() override;

    void update() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subControlRect(QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    bool isEditable() const;
    QStringList elementHints() const override;
    void updateSubElementList() override;
    void drawText(QPainter *painter) const override;
    const QStyleOptionComboBox *m_comboBoxOption = nullptr;
    qreal m_spacing;
    bool m_editable;
};
