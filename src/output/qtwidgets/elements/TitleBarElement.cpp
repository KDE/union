// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "TitleBarElement.h"
#include "SharedNames.h"
#include "UnionStyle.h"
#include "elements/AbstractElement.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

TitleBarElement::TitleBarElement(const QStyleOptionTitleBar *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_titleBarOption(option)
{
    update();
}

TitleBarElement::~TitleBarElement()
{
}

void TitleBarElement::update()
{
    setIcon(m_titleBarOption->icon);
    setText(m_titleBarOption->text);
    updateSubElementList();
    layout();
}

void TitleBarElement::draw(QPainter *painter, DrawEnums enums) const
{
    Q_UNUSED(enums);
    if (!m_isValid) {
        return;
    }

    drawBackground(painter);
    if (!m_titleBarOption->text.isEmpty()
        && (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowTitleHint) || m_titleBarOption->titleBarFlags.testFlag(Qt::WindowSystemMenuHint))) {
        drawText(painter);
    }
    if (!m_titleBarOption->icon.isNull()) {
        drawIconAtRect(painter, m_titleBarOption->icon, m_layoutMap[ElementString::Icon].rect);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowContextHelpButtonHint)) {
        const auto icon = queryIcon(u"help-contextual-symbolic"_s, {ElementString::TitleBar, ElementString::HelpButton});
        drawIconAtRect(painter, icon, m_layoutMap[ElementString::HelpButton].rect);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowMinimizeButtonHint)) {
        const auto icon = queryIcon(u"window-minimize-symbolic"_s, {ElementString::TitleBar, ElementString::MinimizeButton});
        drawIconAtRect(painter, icon, m_layoutMap[ElementString::MinimizeButton].rect);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowMaximizeButtonHint)) {
        const auto icon = queryIcon(u"window-maximize-symbolic"_s, {ElementString::TitleBar, ElementString::MaximizeButton});
        drawIconAtRect(painter, icon, m_layoutMap[ElementString::MaximizeButton].rect);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowCloseButtonHint)) {
        const auto icon = queryIcon(u"window-close-symbolic"_s, {ElementString::TitleBar, ElementString::CloseButton});
        drawIconAtRect(painter, icon, m_layoutMap[ElementString::CloseButton].rect);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowSystemMenuHint)) {
        const auto icon = queryIcon(u"application-menu-symbolic"_s, {ElementString::TitleBar, ElementString::SystemMenu});
        drawIconAtRect(painter, icon, m_layoutMap[ElementString::SystemMenu].rect);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowShadeButtonHint)) {
        const auto icon = queryIcon(u"window-shade-symbolic"_s, {ElementString::TitleBar, ElementString::ShadeButton});
        drawIconAtRect(painter, icon, m_layoutMap[ElementString::ShadeButton].rect);
    }
}

void TitleBarElement::updateSubElementList()
{
    m_subElementList.clear();
    if (!m_titleBarOption->text.isEmpty()
        && (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowTitleHint) || m_titleBarOption->titleBarFlags.testFlag(Qt::WindowSystemMenuHint))) {
        m_subElementList.append(ElementString::Text);
    }
    if (!m_titleBarOption->icon.isNull()) {
        m_subElementList.append(ElementString::Icon);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowContextHelpButtonHint)) {
        m_subElementList.append(ElementString::HelpButton);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowMinimizeButtonHint)) {
        m_subElementList.append(ElementString::MinimizeButton);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowMaximizeButtonHint)) {
        m_subElementList.append(ElementString::MaximizeButton);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowCloseButtonHint)) {
        m_subElementList.append(ElementString::CloseButton);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowSystemMenuHint)) {
        m_subElementList.append(ElementString::SystemMenu);
    }
    if (m_titleBarOption->titleBarFlags.testFlag(Qt::WindowShadeButtonHint)) {
        m_subElementList.append(ElementString::ShadeButton);
    }
}

void TitleBarElement::layout()
{
    // Background and content is separate
    m_backgroundElementList = prepareElements(m_titleBarOption, m_widget, {ElementString::TitleBar});

    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
        m_layoutMap = layoutMap(m_backgroundElementList, m_titleBarOption, m_subElementList);
    }

    m_contentElementList = prepareElements(m_titleBarOption, m_widget, {ElementString::TitleBar});

    if (!m_contentElementList.isEmpty()) {
        m_contentProperties = queryProperties(m_contentElementList);
        m_isValid = true;
    } else {
        m_isValid = false;
        qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
    }
}

QSizeF TitleBarElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    return contentsSizeFromStyle;
}

QRectF TitleBarElement::subControlRect(QStyle::SubControl subControl) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "subControlRect for " << subControl << "is not valid";
        return QRect();
    }

    switch (subControl) {
    case QStyle::SC_TitleBarSysMenu:
        return m_layoutMap[ElementString::SystemMenu].rect;
    case QStyle::SC_TitleBarMinButton:
        return m_layoutMap[ElementString::MinimizeButton].rect;
    case QStyle::SC_TitleBarMaxButton:
        return m_layoutMap[ElementString::MaximizeButton].rect;
    case QStyle::SC_TitleBarCloseButton:
        return m_layoutMap[ElementString::CloseButton].rect;
    case QStyle::SC_TitleBarNormalButton:
        return m_layoutMap[ElementString::NormalButton].rect;
    case QStyle::SC_TitleBarShadeButton:
    case QStyle::SC_TitleBarUnshadeButton:
        return m_layoutMap[ElementString::ShadeButton].rect;
    case QStyle::SC_TitleBarContextHelpButton:
        return m_layoutMap[ElementString::HelpButton].rect;
    case QStyle::SC_TitleBarLabel:
        return m_layoutMap[ElementString::Text].rect;
        break;
    default:
        break;
    }
    return QRect();
}

qreal TitleBarElement::buttonWidth() const
{
    return querySize({ElementString::TitleBar, ElementString::NormalButton}).width();
}

QIcon TitleBarElement::queryIcon(const QString &defaultIconName, const QStringList &targetHierarchy) const
{
    auto props = queryProperties(prepareElements(m_titleBarOption, m_widget, targetHierarchy));
    return m_style->unionIcon(props, defaultIconName);
}

QStringList TitleBarElement::elementHints() const
{
    QStringList hints;
    if (m_titleBarOption->titleBarState & Qt::WindowMaximized) {
        hints.append(u"maximized"_s);
    }
    if (m_titleBarOption->titleBarState & Qt::WindowMinimized) {
        hints.append(u"minimized"_s);
    }
    return hints;
}

qreal TitleBarElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_TitleBarHeight:
        return height();
    case QStyle::PM_TitleBarButtonSize:
        return buttonWidth();
    case QStyle::PM_TitleBarButtonIconSize:
        return iconSize().width();
    default:
        break;
    }
    return 0;
}
