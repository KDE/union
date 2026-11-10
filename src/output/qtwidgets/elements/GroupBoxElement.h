// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "ElementCache.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>

class UnionStyle;
class GroupBoxElement;
namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ComplexControl, QStyle::CC_GroupBox> : public TypeHelper<GroupBoxElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PrimitiveElement, QStyle::PE_FrameGroupBox> : public TypeHelper<GroupBoxElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::ContentsType, QStyle::CT_GroupBox> : public TypeHelper<GroupBoxElement, QStyleOption>{};
/* clang-format on */
}
}

class GroupBoxElement : public AbstractElement
{
    Q_OBJECT

public:
    GroupBoxElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~GroupBoxElement() override;

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawEnums enums) const override;
    QRectF subControlRect(QStyle::SubControl subControl) const override;

private:
    void updateSubElementList() override;
    Union::IdentifierList elementHints() const override;
    void layout() override;
    void drawText(QPainter *painter) const override;
    void drawIcon(QPainter *painter) const override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    const QStyleOptionGroupBox *m_groupBoxOption = nullptr;
    bool m_isCheckable;
};
