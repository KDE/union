/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "QuickStyle.h"

#include <QQmlEngine>

#include <Element.h>
#include <StyleRule.h>
#include <Theme.h>

#include "QuickElement.h"

using namespace Union;

BordersGroup::BordersGroup()
    : QObject()
{
}

Sizes BordersGroup::sizes() const
{
    return m_sizes;
}

QBindable<Sizes> BordersGroup::bindableSizes()
{
    return QBindable<Sizes>(&m_sizes);
}

void BordersGroup::update(const std::optional<Union::BorderDefinition> &borders)
{
    if (!borders.has_value()) {
        m_sizes = Sizes();
        return;
    }

    auto value = borders.value();
    Sizes newSizes;
    newSizes.setLeft(value.left.value_or(LineDefinition()).size);
    newSizes.setRight(value.right.value_or(LineDefinition()).size);
    newSizes.setTop(value.top.value_or(LineDefinition()).size);
    newSizes.setBottom(value.bottom.value_or(LineDefinition()).size);
    m_sizes = newSizes;
}

TextGroup::TextGroup()
    : QObject()
{
}

Qt::Alignment TextGroup::horizontalAlignment() const
{
    return m_horizontalAlignment;
}

QBindable<Qt::Alignment> TextGroup::bindableHorizontalAlignment()
{
    return QBindable<Qt::Alignment>(&m_horizontalAlignment);
}

Qt::Alignment TextGroup::verticalAlignment() const
{
    return m_verticalAlignment;
}

QBindable<Qt::Alignment> TextGroup::bindableVerticalAlignment()
{
    return QBindable<Qt::Alignment>(&m_verticalAlignment);
}

QFont TextGroup::font() const
{
    return m_font;
}

QBindable<QFont> TextGroup::bindableFont()
{
    return QBindable<QFont>(&m_font);
}

void TextGroup::update(const std::optional<Union::TextDefinition> &text)
{
    if (!text.has_value()) {
        return;
    }

    m_horizontalAlignment = text->alignment & Qt::AlignHorizontal_Mask;
    m_verticalAlignment = text->alignment & Qt::AlignVertical_Mask;
    m_font = text->font;
}

QuickStyle::QuickStyle(QObject *parent)
    : QQuickAttachedPropertyPropagator(parent)
{
    m_bordersGroup = std::make_unique<BordersGroup>();
    m_textGroup = std::make_unique<TextGroup>();

    initialize();
}

Sizes QuickStyle::padding() const
{
    return m_padding;
}

QBindable<Sizes> QuickStyle::bindablePadding()
{
    return QBindable<Sizes>(&m_padding);
}

Sizes QuickStyle::margins() const
{
    return m_margins;
}

QBindable<Sizes> QuickStyle::bindableMargins()
{
    return QBindable<Sizes>(&m_margins);
}

BordersGroup *QuickStyle::borders() const
{
    return m_bordersGroup.get();
}

TextGroup *QuickStyle::text() const
{
    return m_textGroup.get();
}

qreal QuickStyle::implicitWidth() const
{
    return m_implicitWidth;
}

QBindable<qreal> QuickStyle::bindableImplicitWidth()
{
    return QBindable<qreal>(&m_implicitWidth);
}

qreal QuickStyle::implicitHeight() const
{
    return m_implicitHeight;
}

QBindable<qreal> QuickStyle::bindableImplicitHeight()
{
    return QBindable<qreal>(&m_implicitHeight);
}

Union::ElementQuery *QuickStyle::query() const
{
    if (m_element) {
        return m_element->query();
    }

    return nullptr;
}

QuickStyle *QuickStyle::qmlAttachedProperties(QObject *parent)
{
    return new QuickStyle(parent);
}

void QuickStyle::attachedParentChange(QQuickAttachedPropertyPropagator *, QQuickAttachedPropertyPropagator *)
{
    update();
}

void QuickStyle::setElement(QuickElement *newElement)
{
    if (newElement == m_element) {
        return;
    }

    if (m_element) {
        m_element->disconnect(this);
    }

    m_element = newElement;

    if (m_element) {
        connect(m_element, &QuickElement::updated, this, &QuickStyle::update);
    }
}

void QuickStyle::update()
{
    auto element = qobject_cast<QuickElement *>(qmlAttachedPropertiesObject<QuickElement>(parent(), false));
    auto parentStyle = attachedParent();

    while (!element && parentStyle) {
        element = qobject_cast<QuickElement *>(qmlAttachedPropertiesObject<QuickElement>(parentStyle->parent(), false));
        parentStyle = parentStyle->attachedParent();
    }

    setElement(element);

    if (!element) {
        return;
    }

    auto query = element->query();
    if (!query || !query->result()) {
        return;
    }

    auto rect = query->boundingRect();

    m_implicitWidth = rect.width();
    m_implicitHeight = rect.height();

    m_margins = Sizes(query->margins());
    m_padding = Sizes(query->padding());

    m_bordersGroup->update(query->border());
    m_textGroup->update(query->text());

    Q_EMIT updated();

    const auto children = attachedChildren();
    for (auto child : children) {
        qobject_cast<QuickStyle *>(child)->update();
    }
}
