// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QStyleOption>

#include <ElementQuery.h>

#include "ElementCache.h"
#include "Layout.h"
#include "StyleGroupWrapper.h"
#include "StyleUtils.h"
#include "TypeMapping.h"
#include "UnionPainter.h"
#include "UnionStyle.h"

namespace Union
{
namespace Widgets
{

static constexpr char UnionElementProperty[] = "Union::Element";

class AbstractElement : public QObject
{
    Q_OBJECT

public:
    enum class AbstractElementFlag {
        ContentsSizeIncludesAveragePadding = 1 << 0,
        DrawMnemonics = 2 << 0,
    };
    Q_DECLARE_FLAGS(AbstractElementFlags, AbstractElementFlag)

    AbstractElement(const UnionStyle *style, QWidget *widget = nullptr);
    ~AbstractElement() override;

    const UnionStyle *style() const;
    QWidget *widget() const;

    AbstractElementFlags flags() const;
    void setFlags(AbstractElementFlags newFlags);
    void setFlag(AbstractElementFlag flag, bool apply = true);
    bool isFlagSet(AbstractElementFlag flag) const;

    Union::Element::Ptr element() const;

    QList<AbstractElement *> childElements() const;
    void addChildElement(AbstractElement *element);
    void removeChildElement(AbstractElement *element);

    template<typename T>
    inline T *firstChildOfType() const
    {
        for (const auto &child : std::as_const(m_childElements)) {
            if (auto typedChild = qobject_cast<T *>(child); typedChild) {
                return typedChild;
            }
        }
        return nullptr;
    }

    StyleGroupWrapper propertyWrapper() const;
    StylePropertyGroupPtr properties() const;

    QRectF rect() const;
    Qt::LayoutDirection direction() const;

    void markDirty();

    virtual void update(const QStyleOption *option);

    virtual void layout();

    using DrawVariant = std::variant<std::nullopt_t, QStyle::ControlElement, QStyle::ComplexControl, QStyle::PrimitiveElement>;
    virtual void draw(QPainter *painter, DrawVariant what) const;

    /*!
     * \brief Return the contents size of the element with padding applied by default.
     */
    virtual QSizeF contentsSize(QSizeF contentsSizeFromStyle) const;

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

    void setElement(const Union::Element::Ptr &element);

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

private:
    const UnionStyle *m_style = nullptr;
    Union::Element::Ptr m_element;
    StyleGroupWrapper m_elementProperties;

    AbstractElementFlags m_flags;
    QWidget *m_widget = nullptr;
    QList<AbstractElement *> m_childElements;

    Layout m_layout;

    QRectF m_rect;
    Qt::LayoutDirection m_direction = Qt::LayoutDirectionAuto;
    bool m_changed = false;
};

// Helper type for std::visit()
template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
}
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Union::Widgets::AbstractElement::AbstractElementFlags);
