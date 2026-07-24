// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include <Element.h>
#include <Style.h>

#include <QCommonStyle>

/*!
 * \brief Provides Union styling for QtWidgets applications.
 */
class UnionStyle : public QCommonStyle
{
    Q_OBJECT
    /* KStyle has custom elements mechanism, disable it. */
    Q_CLASSINFO("X-KDE-CustomElements", "false")

public:
    UnionStyle();

    void drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const override;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;
    void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;

    QSize sizeFromContents(QStyle::ContentsType ct, const QStyleOption *opt, const QSize &contentsSize, const QWidget *widget) const override;
    QRect subElementRect(QStyle::SubElement element, const QStyleOption *option, const QWidget *widget = nullptr) const override;
    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override;
    int styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const override;

    void polish(QApplication *application) override;
    void polish(QWidget *) override;

    void drawIcon(const QRect &rect, const QStyleOption *opt, QPainter *painter, const QIcon &icon, const QWidget *widget = nullptr) const;
    void drawText(const QRect &rect, const QStyleOption *opt, QPainter *painter, const QString &text, const QWidget *widget = nullptr) const;
    void layoutAndDrawIconText(const QStyleOption *opt,
                               QPainter *painter,
                               const QWidget *widget = nullptr,
                               const QIcon &icon = QIcon(),
                               const QString &text = QString()) const;
};
