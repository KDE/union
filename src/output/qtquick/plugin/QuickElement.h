// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QEvent>
#include <QObject>
#include <QProperty>
#include <QQmlParserStatus>
#include <QQuickAttachedPropertyPropagator>
#include <qqmlregistration.h>

#include <Element.h>
#include <ElementQuery.h>
#include <Selector.h>

#include "unionquickimpl_export.h"

class QuickElement;

class StatesGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    StatesGroup(QuickElement *parent);

    /*!
     * \qmlattachedproperty bool Element::states.hovered
     *
     * Is the element hovered?
     *
     * This sets the `Element::State::Hovered` state on the element if set to
     * `true`.
     */
    Q_PROPERTY(bool hovered READ hovered WRITE setHovered NOTIFY hoveredChanged)
    bool hovered() const;
    void setHovered(bool newHovered);
    Q_SIGNAL void hoveredChanged();

    /*!
     * \qmlattachedproperty bool Element::states.activeFocus
     *
     * Does the element have active focus?
     *
     * This sets the `Element::State::ActiveFocus` state on the element if set
     * to `true`.
     */
    Q_PROPERTY(bool activeFocus READ activeFocus WRITE setActiveFocus NOTIFY activeFocusChanged)
    bool activeFocus() const;
    void setActiveFocus(bool newActiveFocus);
    Q_SIGNAL void activeFocusChanged();

    /*!
     * \qmlattachedproperty bool Element::states.visualFocus
     *
     * Does the element have visual focus?
     *
     * This sets the `Element::State::VisualFocus` state on the element if set
     * to `true`.
     */
    Q_PROPERTY(bool visualFocus READ visualFocus WRITE setVisualFocus NOTIFY visualFocusChanged)
    bool visualFocus() const;
    void setVisualFocus(bool newActiveFocus);
    Q_SIGNAL void visualFocusChanged();

    /*!
     * \qmlattachedproperty bool Element::states.pressed
     *
     * Is the element pressed?
     *
     * This sets the `Element::State::Pressed` state on the element if set to
     * `true`.
     */
    Q_PROPERTY(bool pressed READ pressed WRITE setPressed NOTIFY pressedChanged)
    bool pressed() const;
    void setPressed(bool newPressed);
    Q_SIGNAL void pressedChanged();

    /*!
     * \qmlattachedproperty bool Element::states.checked
     *
     * Is the element checked?
     *
     * This sets the `Element::State::Checked` state on the element if set to
     * `true`.
     */
    Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
    bool checked() const;
    void setChecked(bool newChecked);
    Q_SIGNAL void checkedChanged();

    /*!
     * \qmlattachedproperty bool Element::states.enabled
     *
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

    /*!
     * \qmlattachedproperty bool Element::states.highlighted
     *
     * Should this element be highlighted?
     *
     * This sets the `Element::State::Highlighted` state on the element if set
     * to `true`.
     */
    Q_PROPERTY(bool highlighted READ highlighted WRITE setHighlighted NOTIFY highlightedChanged)
    bool highlighted() const;
    void setHighlighted(bool newHighlighted);
    Q_SIGNAL void highlightedChanged();

    /*!
     * \qmlattachedproperty Union::Element::States Element::states.activeStates
     *
     * The set of active states of this StatesGroup.
     *
     * This will contain all the states that are currently active, as set by the
     * other properties.
     */
    Q_PROPERTY(Union::Element::States activeStates READ activeStates NOTIFY activeStatesChanged)
    Union::Element::States activeStates() const;
    Q_SIGNAL void activeStatesChanged();

private:
    friend class QuickElement;

    void setState(Union::Element::State state, bool set);

    // Called by QuickElement upon changes to its underlying Element instance.
    void setActiveStates(Union::Element::States states);

    void emitStateChange(Union::Element::States states);

    QuickElement *m_parent = nullptr;
    Union::Element::States m_activeStates;
};

/*!
 * \qmltype Element
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-core
 *
 * \brief Provides attached properties that provide information about an element
 *        and its hierarchy.
 *
 * Element can be attached to any Item type to provide information about that
 * item and turn it into a styleable element. The information is used by style
 * rules to determine if they should apply to this element. This means you
 * should provide as much information as possible about the element, as that
 * allows style rules to be more specific if needed.
 *
 * To access the properties provided by the matched style rules, use the
 * \l Style attached type.
 *
 * Example usage:
 *
 * \code
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
 * \endcode
 *
 * This would create an element that can be matched by the style rules
 * \c{Type(CustomControl)}, \c{Id(custom-control)}, \c{State(Hovered)} or
 * \c{State(Pressed)} or any combination thereof.
 *
 * \sa Style
 * \sa Union::Element
 * \sa Union::Selector
 */
class UNIONQUICKIMPL_EXPORT QuickElement : public QQuickAttachedPropertyPropagator, public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Element)
    QML_UNCREATABLE("Attached property")
    QML_ATTACHED(QuickElement)
    Q_INTERFACES(QQmlParserStatus)

public:
    QuickElement(QObject *parent = nullptr);

    /*!
     * \qmlattachedproperty string Element::type
     *
     * The type of element.
     */
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    QString type() const;
    void setType(const QString &newType);
    Q_SIGNAL void typeChanged();

    /*!
     * \qmlattachedproperty string Element::elementId
     *
     * A unique id for the element.
     *
     * Note that this is expected to be unique across the entire application,
     * although this is not currently enforced.
     */
    Q_PROPERTY(QString elementId READ elementId WRITE setElementId NOTIFY elementIdChanged)
    QString elementId() const;
    void setElementId(const QString &newId);
    Q_SIGNAL void elementIdChanged();

    /*!
     * \qmlattachedproperty StatesGroup Element::states
     *
     * Grouped property to set the states of the element.
     */
    Q_PROPERTY(StatesGroup *states READ states CONSTANT)
    StatesGroup *states() const;

    /*!
     * \qmlattachedproperty ColorSet Element::colorSet
     *
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

    /*!
     * \qmlattachedproperty QVariantMap Element::attributes
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

    /*!
     * \qmlattachedsignal Element::updated
     *
     * Emitted whenever the structure of elements changed.
     *
     * This will be emitted whenever one of the properties of this element
     * changes, or one of its parent elements changes, including things like
     * state changed. If you make use of `query()` you should re-fetch the query
     * when this signal is emitted.
     */
    Q_SIGNAL void updated();

    static QuickElement *qmlAttachedProperties(QObject *parent);

protected:
    void attachedParentChange(QQuickAttachedPropertyPropagator *, QQuickAttachedPropertyPropagator *) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

    void classBegin() override;
    void componentComplete() override;

private:
    friend class StatesGroup;

    void setActiveStates(Union::Element::States newActiveStates);
    void update();

    std::shared_ptr<Union::Element> m_element;
    std::unique_ptr<StatesGroup> m_statesGroup;

    std::unique_ptr<Union::ElementQuery> m_query;

    bool m_completed = false;
};

class QuickElementUpdatedEvent : public QEvent
{
public:
    QuickElementUpdatedEvent();

    // Todo: Use registerEventType() instead of a hardcoded random offset
    // inline static QEvent::Type s_type = QEvent::Type(QEvent::User + 21860);
    inline static QEvent::Type s_type = QEvent::None;
};
