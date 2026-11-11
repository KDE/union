// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include <ElementQuery.h>
#include <QIcon>
#include <QObject>
#include <QStyle>
#include <QStyleOption>

#include "StyleUtils.h"

class UnionStyle;

namespace Union
{
namespace Widgets
{

class TextSubElement;

static constexpr char UnionElementProperty[] = "Union::Element";

class AbstractElementV2 : public QObject
{
    Q_OBJECT

public:
    enum class AbstractElementFlag {
        ContentsSizeIncludesAveragePadding = 1 << 0,
        DrawMnemonics = 2 << 0,
    };
    Q_DECLARE_FLAGS(AbstractElementFlags, AbstractElementFlag)

    AbstractElementV2(UnionStyle *style, QWidget *widget = nullptr);
    ~AbstractElementV2() override;

    AbstractElementFlags flags() const;
    void setFlags(AbstractElementFlags newFlags);
    void setFlag(AbstractElementFlag flag, bool apply = true);

    virtual Union::Element::Ptr element() const;
    TextSubElement *textSubElement() const;

    virtual void update(const QStyleOption *option);

    using DrawVariant = std::variant<std::nullopt_t, QStyle::ControlElement, QStyle::ComplexControl, QStyle::PrimitiveElement>;
    virtual void draw(QPainter *painter, DrawVariant what) const;

    /*!
     * \brief Return the contents size of the element with padding applied by default.
     */
    virtual QSizeF contentsSize(const QSizeF &contentsSizeFromStyle) const;

    /*!
     * \brief Return a subelement rectangle. If not found, empty QRectF() is returned instead.
     */
    virtual QRectF subElementRect(QStyle::SubElement element) const;

    /*!
     * \brief Return a subcontrol rectangle. If not found, empty QRectF() is returned instead.
     */
    virtual QRectF subControlRect(QStyle::SubControl subControl) const;

    /*!
     * \brief Return the given pixelmetric value. If not found, returns 0
     */
    virtual qreal pixelMetric(QStyle::PixelMetric pixelMetric) const;

    /*!
     * \brief Return the given Stylehint value. If not found, returns 0
     */
    virtual int styleHint(QStyle::StyleHint styleHint) const;

    /*!
     * \brief Return the given standard pixmap. If not found, returns an empty QIcon.
     */
    virtual QIcon standardPixmap(QStyle::StandardPixmap pixmap) const;

protected:
    bool eventFilter(QObject *target, QEvent *event) override;

    virtual void queryProperties();

    void createElement(const Union::Identifier &type);

    /*!
     * \brief Draw whole background of the element.
     */
    virtual void drawBackground(QPainter *painter) const;

    /*!
     * \brief Draw only the background frame of the element.
     */
    virtual void drawFrame(QPainter *painter) const;

    /*!
     * \brief Draw only the background panel of the element.
     */
    virtual void drawPanel(QPainter *painter) const;

    UnionStyle *m_style = nullptr;
    Union::Element::Ptr m_element;
    Union::StylePropertyGroupPtr m_elementProperties;

    bool m_changed = false;

    AbstractElementFlags m_flags;
    QRectF m_rect;

private:
    QWidget *m_widget = nullptr;
    mutable TextSubElement *m_textSubElement = nullptr;
};

}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Union::Widgets::AbstractElementV2::AbstractElementFlags);
