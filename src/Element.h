// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <memory>

#include <QDebug>
#include <QObject>

#include "union_export.h"

namespace Union
{

class ElementPrivate;

/**
 * An element in a hierarchy of elements that is used to match StyleRule instances against.
 *
 * This mainly contains a set of properties to indicate element information,
 * such as what type of element this is. These properties are used to determine
 * if a StyleRule should be applied to an element.
 *
 * Element instances should be created by output plugins to match the hierarchy
 * of objects in the application and supplied values for as many of the
 * properties as possible.
 */
class UNION_EXPORT Element : public QObject, public std::enable_shared_from_this<Element>
{
    Q_OBJECT
public:
    /**
     * A set of flags to indicate an element's state.
     */
    enum class State {
        None,
        Hovered = 1 << 0, ///< The element is hovered.
        ActiveFocus = 1 << 1, ///< The element has active focus, it will receive keyboard input.
        VisualFocus = 1 << 2, ///< The element has active focus and that focus should be visualized.
        Pressed = 1 << 3, ///< The element is being pressed.
        Checked = 1 << 4, ///< The element has a checked state and is currently checked.
        Disabled = 1 << 5, ///< The element is disabled.
        Highlighted = 1 << 6, ///< The element should be highlighted, because it is selected or otherwise.
    };
    Q_DECLARE_FLAGS(States, State)
    Q_FLAG(States)

    /**
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

    using Ptr = std::shared_ptr<Element>;

    Element(std::unique_ptr<ElementPrivate> &&dd);
    ~Element() override;

    /**
     * The type of the element.
     *
     * This can be any arbitrary string that can be used to describe an object
     * in a user interface, such as "Button".
     */
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    QString type() const;
    void setType(const QString &type);
    Q_SIGNAL void typeChanged();

    /**
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

    /**
     * The set of states this element currently has.
     *
     * These are things like "Hovered" and often change fairly often in an
     * application.
     */
    Q_PROPERTY(States states READ states WRITE setStates NOTIFY statesChanged)
    States states() const;
    void setStates(States newStates);
    Q_SIGNAL void statesChanged();

    /**
     * The ColorSet to use to render this element.
     */
    Q_PROPERTY(ColorSet colorSet READ colorSet WRITE setColorSet NOTIFY colorSetChanged)
    ColorSet colorSet() const;
    void setColorSet(ColorSet newColorSet);
    Q_SIGNAL void colorSetChanged();

    /**
     * A list of hints for this element.
     *
     * Hints are used to indicate there's something different about the element
     * which might be useful for style rule matching. This includes things like
     * "is a toolbar at the top or bottom of a view" or "this button should be
     * displayed with a special style". Hints should be fairly static and
     * usually should not be used for state information, the `states` property
     * should cover most of those.
     */
    Q_PROPERTY(QStringList hints READ hints WRITE setHints NOTIFY hintsChanged)
    QStringList hints() const;
    void setHints(const QStringList &newHints);
    Q_SIGNAL void hintsChanged();

    /**
     * A map of extra attributes that can be used for rule matching.
     *
     * This can contain an arbitrary amount of key-value pairs that can be
     * useful for rule matching.
     */
    Q_PROPERTY(QVariantMap attributes READ attributes WRITE setAttributes NOTIFY attributesChanged)
    QVariantMap attributes() const;
    void setAttributes(const QVariantMap &attributes);
    Q_SIGNAL void attributesChanged();

    Q_SIGNAL void updated();

    bool hasAttribute(const QString &name) const;
    QVariant attribute(const QString &name) const;

    static Element::Ptr create();

private:
    const std::unique_ptr<ElementPrivate> d;
};

using ElementList = QList<Element::Ptr>;

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Union::Element::States)

UNION_EXPORT QDebug operator<<(QDebug debug, Union::Element::Ptr element);
