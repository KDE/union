/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QObject>
#include <QProperty>
#include <QQuickAttachedPropertyPropagator>
#include <qqmlregistration.h>

#include <Element.h>
#include <ElementQuery.h>
#include <Selector.h>

class QuickElement;

/**
 * Different states an element can be in.
 *
 * This is used as a grouped property. It should be used to bind states from
 * Items to expose them to the style.
 */
class StatesGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    StatesGroup(QuickElement *parent);

    /**
     * Is the element hovered?
     *
     * This sets the `Element::State::Hovered` state on the element if set to
     * `true`.
     */
    Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)
    bool hovered() const;
    void setHovered(bool newHovered);
    Q_SIGNAL void hoveredChanged();

    /**
     * Does the element have active focus?
     *
     * This sets the `Element::State::ActiveFocus` state on the element if set
     * to `true`.
     */
    Q_PROPERTY(bool activeFocus READ activeFocus WRITE setActiveFocus NOTIFY activeFocusChanged)
    bool activeFocus() const;
    void setActiveFocus(bool newActiveFocus);
    Q_SIGNAL void activeFocusChanged();

    /**
     * Does the element have visual focus?
     *
     * This sets the `Element::State::VisualFocus` state on the element if set
     * to `true`.
     */
    Q_PROPERTY(bool visualFocus READ visualFocus WRITE setVisualFocus NOTIFY visualFocusChanged)
    bool visualFocus() const;
    void setVisualFocus(bool newActiveFocus);
    Q_SIGNAL void visualFocusChanged();

    /**
     * Is the element pressed?
     *
     * This sets the `Element::State::Pressed` state on the element if set to
     * `true`.
     */
    Q_PROPERTY(bool pressed READ pressed WRITE setPressed NOTIFY pressedChanged)
    bool pressed() const;
    void setPressed(bool newPressed);
    Q_SIGNAL void pressedChanged();

    /**
     * Is the element checked?
     *
     * This sets the `Element::State::Checked` state on the element if set to
     * `true`.
     */
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    bool checked() const;
    void setChecked(bool newChecked);
    Q_SIGNAL void checkedChanged();

    /**
     * Is the element enabled?
     *
     * This sets the `Element::State::Disabled` state on the element if set
     * to `false`.
     *
     * Note that this behavior is inverted from most other states here, as
     * "Enabled" is expected to be the default state of an element.
     */
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    bool enabled() const;
    void setEnabled(bool newEnabled);
    Q_SIGNAL void enabledChanged();

    /**
     * Should this element be highlighted?
     *
     * This sets the `Element::State::Highlighted` state on the element if set
     * to `true`.
     */
    Q_PROPERTY(bool highlighted READ highlighted WRITE setHighlighted NOTIFY highlightedChanged)
    bool highlighted() const;
    void setHighlighted(bool newHighlighted);
    Q_SIGNAL void highlightedChanged();

    /**
     * The set of active states of this StatesGroup.
     *
     * This will contain all the states that are currently active, as set by the
     * other properties.
     */
    Q_PROPERTY(Union::Element::States activeStates READ activeStates NOTIFY activeStatesChanged)
    Union::Element::States activeStates() const;
    Q_SIGNAL void activeStatesChanged();

private:
    void setState(Union::Element::State state, bool set);

    QuickElement *m_parent = nullptr;
    Union::Element::States m_activeStates;
};

/**
 * An attached property that provides information about an element and its hierarchy.
 *
 * This is used to provide metadata about an element that can then be used by
 * the theme to match style rules. You should provide as much metadata about
 * elements as possible, so style rules can be more specific if needed.
 *
 * To match style rules, all the metadata from an instance of this class along
 * with its parent elements is communicated to the theme and used to match style
 * rules. The matched style rules are exposed through `Union::ElementQuery`
 * which allows for resolving different properties against different matched
 * rules. To access these properties from the QML side, use the `QuickStyle`
 * attached property.
 *
 * Example usage:
 *
 * ```
 * Rectangle {
 *     id: element
 *
 *     Union.Element.type: "CustomControl"
 *     Union.Element.elementId: "custom-control"
 *     Union.Element.states {
 *         hovered: hover.hovered
 *         pressed: press.active
 *     }
 *
 *     color: Union.Style.color
 *
 *     HoverHandler {
 *         id: hover
 *     }
 *
 *     TapHandler {
 *         id: press
 *     }
 * }
 * ```
 *
 * This would create an element that can be matched by the style rules
 * `Type(CustomControl)`, `Id(custom-control)`, `State(Hovered)` or
 * `State(Pressed)` or any combination of those.
 *
 * \sa QuickStyle
 * \sa Union::Element
 * \sa Union::Selector
 */
class QuickElement : public QQuickAttachedPropertyPropagator
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Element)
    QML_ATTACHED(QuickElement)

public:
    QuickElement(QObject *parent = nullptr);

    /**
     * The type of element.
     */
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    QString type() const;
    void setType(const QString &newType);
    Q_SIGNAL void typeChanged();

    /**
     * A unique id for the element.
     *
     * Note that this is expected to be unique across the entire application,
     * although this is not currently enforced.
     */
    Q_PROPERTY(QString elementId READ elementId WRITE setElementId NOTIFY elementIdChanged)
    QString elementId() const;
    void setElementId(const QString &newId);
    Q_SIGNAL void elementIdChanged();

    /**
     * Grouped property to set the states of the element.
     */
    Q_PROPERTY(StatesGroup *states READ states CONSTANT)
    StatesGroup *states() const;

    /**
     * The color set to use for this element.
     *
     * The color set determines a specific set of system colors that should be
     * used in the appropriate context.
     *
     * \sa Union::Element::ColorSet
     */
    Q_PROPERTY(Union::Element::ColorSet colorSet READ colorSet WRITE setColorSet NOTIFY colorSetChanged)
    Union::Element::ColorSet colorSet() const;
    void setColorSet(Union::Element::ColorSet newColorSet);
    Q_SIGNAL void colorSetChanged();

    /**
     * A list of extra hints to provide to the theme.
     *
     * These should be set if there are extra criteria to be used to style an
     * element, for example error/warning/information variants.
     */
    Q_PROPERTY(QStringList hints READ hints WRITE setHints NOTIFY hintsChanged)
    QStringList hints() const;
    void setHints(const QStringList &newHints);
    Q_SIGNAL void hintsChanged();

    /**
     * A map of extra attributes to provide to the theme.
     *
     * These can be used to provide extra criteria to be used to style an
     * element. They can be used to do things like select a specific theme to
     * use for an element.
     */
    Q_PROPERTY(QVariantMap attributes READ attributes WRITE setAttributes NOTIFY attributesChanged)
    QVariantMap attributes() const;
    void setAttributes(const QVariantMap &newAttributes);
    Q_SIGNAL void attributesChanged();

    /**
     * The query built from this element and its parents.
     */
    Union::ElementQuery *query() const;

    /**
     * Emitted whenever the structure of elements changed.
     *
     * This will be emitted whenever one of the properties of this element
     * changes, or one of its parent elements changes, including things like
     * state changed. If you make use of `query()` you should re-fetch the query
     * when this signal is emitted.
     */
    Q_SIGNAL void updated();

    /**
     * Attached property getter.
     */
    static QuickElement *qmlAttachedProperties(QObject *parent);

protected:
    void attachedParentChange(QQuickAttachedPropertyPropagator *, QQuickAttachedPropertyPropagator *) override;

private:
    friend class StatesGroup;

    void setActiveStates(Union::Element::States newActiveStates);
    void update();

    std::shared_ptr<Union::Element> m_element;
    std::unique_ptr<StatesGroup> m_statesGroup;

    std::unique_ptr<Union::ElementQuery> m_query;

    QPropertyNotifier m_activeStatesNotifier;
};
