// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Joshua Goins <josh@redstrate.com>

#pragma once

#include "LruCache.h"
#include <Element.h>
#include <Style.h>

#include "elements/AbstractElement.h"
#include "elements/CheckBoxElement.h"
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
    SubControl hitTestComplexControl(ComplexControl, const QStyleOptionComplex *, const QPoint &, const QWidget *) const override;
    void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;

    QSize sizeFromContents(QStyle::ContentsType contentsType, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const override;
    QRect subElementRect(QStyle::SubElement element, const QStyleOption *option, const QWidget *widget = nullptr) const override;
    QRect
    subControlRect(ComplexControl complexControl, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget = nullptr) const override;

    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override;
    int styleHint(StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const override;

    void polish(QApplication *application) override;
    void polish(QWidget *) override;

    bool eventFilter(QObject *object, QEvent *event) override;

    void drawItemText(QPainter *painter,
                      const QRect &rect,
                      int flags,
                      const QPalette &pal,
                      bool enabled,
                      const QString &text,
                      QPalette::ColorRole textRole = QPalette::NoRole) const override;

    QIcon unionIcon(Union::Properties::StylePropertyGroup *properties, const QString &defaultName) const;

private:
    bool m_showMnemonics;
    void setMnemonics(bool enabled);
    mutable Union::LruCache<size_t, std::shared_ptr<AbstractElement>> m_elementCache;

    template<typename ElementType, typename StyleOptionType>
    std::shared_ptr<ElementType> cachedElement(size_t hash, const QStyleOption *option, const QWidget *widget) const
    {
        if (const auto opt = qstyleoption_cast<const StyleOptionType *>(option)) {
            std::shared_ptr<ElementType> element;
            if (m_elementCache.contains(hash)) {
                element = std::static_pointer_cast<ElementType>(m_elementCache.value(hash).value());
                element->update();
            } else {
                element = std::make_shared<ElementType>(opt, this, widget);
                m_elementCache.insert(hash, element);
            }
            return element;
        }
        return nullptr;
    }
};
