// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "ElementCache.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

class UnionStyle;
class ComboBoxElement;
namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ComplexControl, QStyle::CC_ComboBox> : public TypeHelper<ComboBoxElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::ControlElement, QStyle::CE_ComboBoxLabel> : public TypeHelper<ComboBoxElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_ComboBox> : public TypeHelper<ComboBoxElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_ComboBoxFrameWidth> : public TypeHelper<ComboBoxElement, QStyleOption>{};
/* clang-format on */
}
}

class ComboBoxElement : public AbstractElement
{
    Q_OBJECT

public:
    ComboBoxElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~ComboBoxElement() override;

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subControlRect(QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    bool isEditable() const;
    Union::IdentifierList elementHints() const override;
    void updateSubElementList() override;
    void drawText(QPainter *painter) const override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    const QStyleOptionComboBox *m_comboBoxOption = nullptr;
    bool m_editable;
    bool m_hasFrame;

    Union::ElementList m_indicatorElementList;
    Union::StylePropertyGroupPtr m_indicatorProperties = nullptr;
};
