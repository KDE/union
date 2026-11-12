// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"
#include "ElementCache.h"
#include <QIcon>
#include <QObject>
#include <QStyleOption>
#include <qstyleoption.h>

namespace Union
{
namespace Widgets
{

class UnionStyle;
class TitleBarElement;

namespace ElementCache
{
namespace detail
{
/* clang-format off */
        template<> struct EnumToType<QStyle::ComplexControl, QStyle::CC_TitleBar> : public TypeHelper<TitleBarElement, QStyleOption>{};

        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_TitleBarHeight> : public TypeHelper<TitleBarElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_TitleBarButtonSize> : public TypeHelper<TitleBarElement, QStyleOption>{};
        template<> struct EnumToType<QStyle::PixelMetric, QStyle::PM_TitleBarButtonIconSize> : public TypeHelper<TitleBarElement, QStyleOption>{};
/* clang-format on */
}
}

class TitleBarElement : public AbstractElement
{
    Q_OBJECT

public:
    TitleBarElement(const QStyleOption *option, const UnionStyle *style, const QWidget *widget = nullptr);
    ~TitleBarElement() override;

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

    QRectF subControlRect(QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const override;

    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    Union::IdentifierList elementHints() const override;
    void updateSubElementList() override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    const QStyleOptionTitleBar *m_titleBarOption = nullptr;
    QIcon queryIcon(const QString &defaultIconName, const QStringList &targetHierarchy) const;
    qreal buttonWidth() const;
};

}
}
