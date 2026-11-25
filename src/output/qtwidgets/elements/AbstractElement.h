// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <optional>

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
        DrawMnemonics = 1 << 1,
        SubElement = 1 << 2,
    };
    Q_DECLARE_FLAGS(AbstractElementFlags, AbstractElementFlag)

    AbstractElement(const UnionStyle *style, QWidget *widget);

    const UnionStyle *style() const;
    QWidget *widget() const;

    bool isValid() const;

    AbstractElementFlags flags() const;
    void setFlags(AbstractElementFlags newFlags);
    void setFlag(AbstractElementFlag flag, bool apply = true);
    bool isFlagSet(AbstractElementFlag flag) const;

    Union::Element::Ptr element() const;
    Union::Element::Ptr ensureElement();

    QList<AbstractElement *> childElements() const;
    void addChildElement(AbstractElement *element);
    void removeChildElement(AbstractElement *element);

    template<typename T>
    inline T *firstChildOfType(bool create = false)
    {
        for (const auto &child : std::as_const(m_childElements)) {
            if (auto typedChild = qobject_cast<T *>(child); typedChild) {
                return typedChild;
            }
        }

        if (create) {
            auto child = new T{style(), widget()};
            addChildElement(child);
            return child;
        }

        return nullptr;
    }

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

    template<typename T, typename F>
        requires std::is_invocable_v<F, T *>
    inline void withFirstChild(bool shouldExist, F &&function)
    {
        if (shouldExist) {
            auto child = firstChildOfType<T>(true);
            function(child);
        } else {
            auto child = firstChildOfType<T>(false);
            if (child) {
                removeChildElement(child);
            }
        }
    }

    StyleGroupWrapper propertyWrapper() const;
    StylePropertyGroupPtr properties() const;

    QRectF rect() const;
    void setRect(const QRectF &newRect);

    Qt::LayoutDirection direction() const;

    void markDirty();

    virtual void update(const QStyleOption *option);

    virtual void layout();

    using DrawVariant = std::variant<std::nullopt_t, QStyle::ControlElement, QStyle::ComplexControl, QStyle::PrimitiveElement>;
    virtual void draw(QPainter *painter, DrawVariant what = std::nullopt) const;

    /*!
     * \brief Return the contents size of the element with padding applied by default.
     */
    virtual QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const;

    /*!
     * \brief Return a subelement rectangle. If not found, empty QRectF() is returned instead.
     */
    virtual QRectF subElementRect(const QStyleOption *option, QStyle::SubElement element) const;

    /*!
     * \brief Return a subcontrol rectangle. If not found, empty QRectF() is returned instead.
     */
    virtual QRectF subControlRect(const QStyleOption *option, QStyle::SubControl subControl) const;

    /*!
     * \brief Return the given pixelmetric value. If not found, returns 0
     */
    virtual std::optional<qreal> pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const;

    /*!
     * \brief Return the given Stylehint value. If not found, returns 0
     */
    virtual int styleHint(const QStyleOption *option, QStyle::StyleHint styleHint) const;

    /*!
     * \brief Return the given standard pixmap. If not found, returns an empty QIcon.
     */
    virtual QIcon standardPixmap(const QStyleOption *option, QStyle::StandardPixmap pixmap) const;

protected:
    bool eventFilter(QObject *target, QEvent *event) override;

    virtual void queryProperties();

    void setElement(const Union::Element::Ptr &element);

    /*!
     * \brief Draw whole background of the element.
     */
    virtual void drawBackground(QPainter *painter, UnionPainter::Parts parts = UnionPainter::Parts::All) const;

    template<typename FilterType = void>
    void drawChildren(QPainter *painter, DrawVariant what = std::nullopt) const
    {
        for (auto child : std::as_const(m_childElements)) {
            if constexpr (std::is_same_v<FilterType, void>) {
                child->draw(painter, what);
            } else if (qobject_cast<FilterType *>(child)) {
                child->draw(painter, what);
            }
        }
    }

    QRectF visualRect(AbstractElement *childElement) const;

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
