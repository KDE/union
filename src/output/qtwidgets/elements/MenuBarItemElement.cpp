// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "MenuBarItemElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

MenuBarItemElement::MenuBarItemElement(const QStyleOptionMenuItem *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_menuItemOption(option)
{
    update();
}

MenuBarItemElement::~MenuBarItemElement()
{
}

void MenuBarItemElement::update()
{
    setIndicator(QIcon());
    setIcon(m_menuItemOption->icon);
    setText(m_menuItemOption->text);
    updateSubElementList();
    layout();
}

void MenuBarItemElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_MenuBarItem:
        drawBackground(painter);
        drawIcon(painter);
        drawText(painter);
        drawIndicator(painter);
        break;
    case QStyle::CE_MenuBarEmptyArea:
        break;
    }
}

void MenuBarItemElement::updateSubElementList()
{
    m_subElementList.clear();
    m_subElementList.append(ElementString::Text);
    if (!m_menuItemOption->icon.isNull()) {
        m_subElementList.append(ElementString::Icon);
    }
}

void MenuBarItemElement::layout()
{
    // Background and content is separate
    m_backgroundElementList = prepareElements(m_menuItemOption, m_widget, {ElementString::MenuBarItem});
    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
    }

    m_contentElementList = prepareElements(m_menuItemOption, m_widget, {ElementString::MenuBarItem});
    if (!m_contentElementList.isEmpty()) {
        m_contentProperties = queryProperties(m_contentElementList);
    }

    m_layoutMap = layoutMap(m_contentElementList, m_menuItemOption, m_subElementList);
    m_isValid = true;
}

QStringList MenuBarItemElement::elementHints() const
{
    QStringList hints;
    if (m_menuItemOption->checked) {
        hints.append(u"with-submenu"_s);
    }
    if (m_menuItemOption->menuItemType == QStyleOptionMenuItem::Separator && !m_menuItemOption->text.isEmpty()) {
        hints.append(u"with-title"_s);
    }
    return hints;
}
