// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <memory>

#include <QDebug>
#include <QObject>

#include "union_export.h"

/*!
 * \namespace Union
 * \inmodule core
 * \ingroup core-namespaces
 *
 * All public C++ types for the Union library.
 */
namespace Union
{

class ElementPrivate;

/*!
 * \class Union::Element
 * \inmodule core
 * \ingroup core-classes
 *
 * \brief An element in a tree of elements that is used to match StyleRules.
 *
 * This class mainly contains a set of properties to indicate element
 * information, such as what type of element this is. These properties are used
 * to determine if a StyleRule should be applied to an element.
 *
 * Element instances should be created by output plugins to match the hierarchy
 * of objects in the application and supplied values for as many of the
 * properties as possible.
 *
 * \note Element inherits from \c{std::enable_shared_from_this} as it is
 * intended to be stored as a \c{std::shared_ptr}. To create a new instance of
 * Element, use \l Element::create() as the constructor is semi-private.
 */
class UNION_EXPORT Element : public QObject, public std::enable_shared_from_this<Element>
{
    Q_OBJECT
public:
    /*!
     * \enum Union::Element::State
     *
     * A set of flags to indicate an element's state.
     *
     * \value None
     *      No State is currently active.
     * \value Hovered
     *      The element is hovered.
     * \value ActiveFocus
     *      The element has active focus, it will receive keyboard input.
     * \value VisualFocus
     *      The element has active focus and that focus should be visualized.
     * \value Pressed
     *      The element is being pressed.
     * \value Checked
     *      The element has a checked state and is currently checked.
     * \value Disabled
     *      The element is disabled.
     * \value Highlighted
     *      The element should be highlighted, because it is selected or
     *      otherwise needs to stand out.
     */
    enum class State {
        None,
        Hovered = 1 << 0,
        ActiveFocus = 1 << 1,
        VisualFocus = 1 << 2,
        Pressed = 1 << 3,
        Checked = 1 << 4,
        Disabled = 1 << 5,
        Highlighted = 1 << 6,
    };
    Q_DECLARE_FLAGS(States, State)
    Q_FLAG(States)

    /*!
     * \enum Union::Element::ColorSet
     *
     * Which set of colours should be used to render the element.
     */
    enum class ColorSet {
        None,
        View,
        Window,
        Button,
        Selection,
        Tooltip,
        Complementary,
        Header,
    };
    Q_ENUM(ColorSet)

    /*!
     * \typealias Union::Element::Ptr
     *
     * A shorthand for \c{std::shared_ptr<Element>}.
     */
    using Ptr = std::shared_ptr<Element>;

    Element(std::unique_ptr<ElementPrivate> &&dd);
    ~Element() override;

    /*!
     * \property Union::Element::type
     *
     * The type of the element.
     *
     * This can be any arbitrary string that can be used to describe an object
     * in a user interface, such as "Button".
     */
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    QString type() const;
    void setType(const QString &type);
    Q_SIGNAL void typeChanged();

    /*!
     * \property Union::Element::id
     *
     * A unique ID to identify a specific element.
     *
     * This can be any string that can be used to identify a specific element in
     * an application. These IDs should be unique to ensure style rules intended
     * to style a specific element aren't suddenly applied to other elements.
     */
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    QString id() const;
    void setId(const QString &newId);
    Q_SIGNAL void idChanged();

    /*!
     * \property Union::Element::states
     *
     * The set of states this element currently has.
     *
     * These are things like "Hovered" and often change fairly often in an
     * application.
     */
    Q_PROPERTY(States states READ states WRITE setStates NOTIFY statesChanged)
    States states() const;
    void setStates(States newStates);
    Q_SIGNAL void statesChanged();

    /*!
     * \property Union::Element::colorSet
     *
     * The ColorSet to use to render this element.
     */
    Q_PROPERTY(ColorSet colorSet READ colorSet WRITE setColorSet NOTIFY colorSetChanged)
    ColorSet colorSet() const;
    void setColorSet(ColorSet newColorSet);
    Q_SIGNAL void colorSetChanged();

    /*!
     * \property Union::Element::hints
     *
     * A set of hints for this element.
     *
     * Hints are used to indicate there's something different about the element
     * which might be useful for style rule matching. This includes things like
     * "is a toolbar at the top or bottom of a view" or "this button should be
     * displayed with a special style". Hints should be fairly static and
     * usually should not be used for state information, the \l states property
     * should cover most of those.
     */
    Q_PROPERTY(QSet<QString> hints READ hints WRITE setHints NOTIFY hintsChanged)
    QSet<QString> hints() const;
    void setHints(const QSet<QString> &newHints);
    Q_SIGNAL void hintsChanged();

    /*!
     * Add or remove a single hint.
     *
     * This adds the hint \p name if \p present is \c{true} and the hint was not
     * yet added. It will remove the hint if \p present is \c{false} and the
     * hint is present.
     */
    void setHint(const QString &name, bool present = true);

    /*!
     * Check if a hint is set.
     *
     * Returns \c{true} if \p name is present, otherwise returns \c{false}.
     */
    bool hasHint(const QString &name);

    /*!
     * \property Union::Element::attributes
     *
     * A map of extra attributes that can be used for rule matching.
     *
     * This can contain an arbitrary amount of key-value pairs that can be
     * useful for rule matching.
     */
    Q_PROPERTY(QVariantMap attributes READ attributes WRITE setAttributes NOTIFY attributesChanged)
    QVariantMap attributes() const;
    void setAttributes(const QVariantMap &attributes);
    Q_SIGNAL void attributesChanged();

    /*!
     * Set a single attribute value.
     */
    void setAttribute(const QString &name, const QVariant &value);

    /*!
     * Check if a certain attribute is set on this element.
     *
     * \a name The name of the attribute to check.
     *
     * Returns true if the attribute is set, false if not.
     */
    bool hasAttribute(const QString &name) const;

    /*!
     * Get the value of a certain attribute.
     *
     * \a name The name of the attribute to get.
     *
     * Returns the value of the attribute if it is set, or an empty QVariant if
     * not.
     */
    QVariant attribute(const QString &name) const;

    /*!
     * Emitted whenever any of the properties of the element have changed.
     */
    Q_SIGNAL void updated();

    /*!
     * Returns a string representation of this Element.
     */
    QString toString() const;

    /*!
     * Returns a newly created instance of Element.
     */
    static Element::Ptr create();

private:
    const std::unique_ptr<ElementPrivate> d;
};

/*!
 * \typealias Union::ElementList
 * \relates Union::Element
 *
 * A list of elements.
 */
using ElementList = QList<Element::Ptr>;

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Union::Element::States)

/*!
 * \relates Union::Element
 *
 * Implements QDebug support for Union::Element.
 */
UNION_EXPORT QDebug operator<<(QDebug debug, Union::Element::Ptr element);

/*!
 * \relates Union::Element
 *
 * Specialization of std::formatter to support formatting an element as string.
 */
template<>
struct std::formatter<Union::Element::Ptr, char> : public std::formatter<std::string, char> {
    template<class FormatContext>
    FormatContext::iterator format(const Union::Element::Ptr &value, FormatContext &context) const
    {
        auto string = value->toString().toStdString();
        return std::formatter<std::string, char>::format(string, context);
    }
};

/*!
 */
template<>
struct std::formatter<Union::ElementList, char> : public std::formatter<std::string, char> {
    template<class FormatContext>
    FormatContext::iterator format(const Union::ElementList &value, FormatContext &context) const
    {
        using namespace Qt::StringLiterals;

        QStringList elements;
        std::ranges::transform(value, std::back_inserter(elements), &Union::Element::toString);
        auto string = QString(u"ElementList("_s + elements.join(u", "_s) + u")"_s).toStdString();

        return std::formatter<std::string, char>::format(string, context);
    }
};
