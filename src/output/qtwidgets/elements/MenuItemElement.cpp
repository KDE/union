// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "MenuItemElement.h"
#include "PropertiesTypes.h"
#include "SharedNames.h"
#include "StyleUtils.h"
#include "UnionStyle.h"
#include "elements/AbstractElement.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>

using namespace Qt::StringLiterals;

MenuItemElement::MenuItemElement(const QStyleOptionMenuItem *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_menuItemOption(option)
    , m_isSeparator(false)
    , m_hasSubMenu(false)
    , m_hasCheckBox(false)
    , m_hasRadioButton(false)
    , m_shortcutText(QString())
{
    update();
}

MenuItemElement::~MenuItemElement()
{
}

void MenuItemElement::update()
{
    m_isSeparator = (m_menuItemOption->menuItemType == QStyleOptionMenuItem::Separator);
    m_hasSubMenu = (m_menuItemOption->menuItemType == QStyleOptionMenuItem::SubMenu);
    m_hasCheckBox = (m_menuItemOption->checkType == QStyleOptionMenuItem::NonExclusive);
    m_hasRadioButton = (m_menuItemOption->checkType == QStyleOptionMenuItem::Exclusive);
    m_menuHMargin = m_style->pixelMetric(QStyle::PM_MenuHMargin, m_styleOption, m_widget);
    m_menuVMargin = m_style->pixelMetric(QStyle::PM_MenuVMargin, m_styleOption, m_widget);

    setIndicator(QIcon());
    if (m_hasSubMenu) {
        m_indicatorElementList = prepareElements(m_menuItemOption, m_widget, {ElementString::MenuItem});
        m_indicatorProperties = queryProperties(m_indicatorElementList);
        setIndicator(m_style->unionIcon(m_indicatorProperties, u"arrow-right-symbolic"_s));
    }
    if (m_hasCheckBox || m_hasRadioButton) {
        QStyleOptionButton button;
        button.initFrom(m_widget);
        m_checkElementList =
            prepareElements(&button, m_widget, {m_hasCheckBox ? ElementString::CheckBox : ElementString::RadioButton, ElementString::Indicator});
        m_checkProperties = queryProperties(m_checkElementList);
    }

    setIcon(m_menuItemOption->icon);
    setText(m_menuItemOption->text);

    updateSubElementList();
    layout();
}

void MenuItemElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }

    switch (enums.ControlElement) {
    case QStyle::CE_MenuItem:
        drawBackground(painter);
        drawIcon(painter);
        drawText(painter);
        drawIndicator(painter);
        if (m_hasCheckBox || m_hasRadioButton) {
            drawBackgroundRectangle(painter, m_layoutMap[ElementString::Indicator].rect, m_checkProperties);
        }
        break;
    }
}

void MenuItemElement::updateSubElementList()
{
    m_subElementList.clear();
    if (!m_menuItemOption->text.isEmpty()) {
        m_subElementList.append(ElementString::Text);
    }
    if (!m_menuItemOption->icon.isNull()) {
        m_subElementList.append(ElementString::Icon);
    }
    if (m_menuItemOption->menuHasCheckableItems) {
        m_subElementList.append(ElementString::CheckBox);
    }
    if (m_menuItemOption->menuItemType == QStyleOptionMenuItem::SubMenu) {
        m_subElementList.append(ElementString::Arrow);
    }
    if (m_menuItemOption->menuItemType == QStyleOptionMenuItem::Separator) {
        m_subElementList.append(ElementString::MenuSeparator);
    }
    if (m_menuItemOption->checkType != QStyleOptionMenuItem::NotCheckable) {
        m_subElementList.append(ElementString::Indicator);
    }
}

void MenuItemElement::layout()
{
    // Background and content is separate
    m_backgroundElementList = prepareElements(m_menuItemOption, m_widget, {ElementString::MenuItem});

    if (!m_backgroundElementList.isEmpty()) {
        m_backgroundProperties = queryProperties(m_backgroundElementList);
    }

    QStringList subElements;
    QString itemText = text();
    if (m_isSeparator) {
        if (hasText()) {
            subElements.append(ElementString::Text);
        }
    } else {
        if (m_hasCheckBox || m_hasRadioButton) {
            subElements.append(ElementString::Indicator);
        }
        if (hasIcon()) {
            subElements.append(ElementString::Icon);
        }
        if (hasText()) {
            subElements.append(ElementString::Text);
            const int tabPosition(itemText.indexOf(QLatin1Char('\t')));
            if (tabPosition >= 0) {
                subElements.append(ElementString::ShortcutText);
                m_shortcutText = itemText.mid(tabPosition + 1);
                m_text = itemText.left(tabPosition);
            } else {
                m_shortcutText = QString();
            }
        } else {
            m_shortcutText = QString();
        }
        if (hasIndicator()) {
            subElements.append(ElementString::Arrow);
        }

        if (subElements.empty()) {
            m_isValid = false;
            return;
        }
    }

    if (m_isSeparator) {
        m_contentElementList = prepareElements(m_menuItemOption, m_widget, {ElementString::MenuSeparator});
    } else {
        m_contentElementList = prepareElements(m_menuItemOption, m_widget, {ElementString::MenuItem});
    }
    if (!m_contentElementList.isEmpty()) {
        m_contentProperties = queryProperties(m_contentElementList);
    }

    auto adjustedOpt = *m_menuItemOption;
    adjustedOpt.rect = adjustedRect(m_menuItemOption->rect).toRect();
    m_layoutMap = layoutMap(m_contentElementList, &adjustedOpt, subElements);
    m_isValid = true;
}

QSizeF MenuItemElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    QSizeF preferredSize = contentsSizeFromStyle;
    // Handle separator separately (pun not intended)
    if (m_menuItemOption) {
        if (m_menuItemOption->menuItemType == QStyleOptionMenuItem::Separator) {
            if (m_contentProperties->layout()) {
                int width = m_contentProperties->layout()->width().value_or(1);
                int height = m_contentProperties->layout()->height().value_or(1);
                if (hasText()) {
                    if (preferredSize.width() > width) {
                        width = preferredSize.width();
                    }
                    if (preferredSize.height() > height) {
                        height = preferredSize.height();
                    }
                }
                QSizeF separatorSize(width, height);
                if (m_contentProperties->layout()->inset()) {
                    auto pad = m_contentProperties->layout()->inset()->toMargins();
                    separatorSize = separatorSize.expandedTo(QSize(pad.left() + pad.right(), pad.top() + pad.bottom()));
                }

                // If we have text, we want to apply padding normally. If not, we want to remove padding and utilize the insets.
                preferredSize = applyPaddingToSize(separatorSize, hasText() ? PaddingDirection::Outward : PaddingDirection::Inward, m_contentProperties);
            }
        } else {
            if (m_contentProperties->layout()) {
                QSizeF itemSize(contentsSizeFromStyle);
                int spacing = m_contentProperties->layout()->spacing().value_or(0);
                if (hasIcon()) {
                    itemSize.rwidth() += m_layoutMap[ElementString::Icon].rect.width() + spacing;
                }
                if (hasIndicator()) {
                    itemSize.rwidth() += m_layoutMap[ElementString::Arrow].rect.width() + spacing;
                }
                if (m_menuItemOption->menuHasCheckableItems) {
                    const bool exclusive = (m_menuItemOption->checkType == QStyleOptionMenuItem::Exclusive);
                    itemSize.rwidth() +=
                        m_style->pixelMetric(exclusive ? QStyle::PM_ExclusiveIndicatorWidth : QStyle::PM_IndicatorWidth, m_menuItemOption, m_widget) + spacing;
                }
                preferredSize = applyPaddingToSize(itemSize);
            }
        }
    }
    // Adjust the contents according to the menu margins.
    // Otherwise they wont be centered properly.
    preferredSize.rwidth() += m_menuHMargin;
    preferredSize.rheight() += m_menuVMargin;
    return preferredSize;
}

void MenuItemElement::drawBackground(QPainter *painter) const
{
    // Draw the  separator rectangle full width if its set to fill
    if (m_isSeparator) {
        QRectF rect = backgroundRectangle(m_menuItemOption, m_contentProperties);
        if (m_contentProperties->layout()
            && m_contentProperties->layout()->alignment()->horizontal().value_or(Union::Properties::Alignment::Fill) == Union::Properties::Alignment::Fill) {
            rect = centerRect(rect,
                              m_contentProperties->layout()->width().value_or(m_menuItemOption->rect.width()),
                              m_contentProperties->layout()->height().value_or(1));
        }
        // Adjust only the width, as we want to keep the height as is
        const auto frameWidth = m_style->pixelMetric(QStyle::PM_MenuPanelWidth, m_styleOption, m_widget);
        drawBackgroundRectangle(painter, rect.adjusted(0, 0, -(m_menuHMargin - frameWidth), 0), m_contentProperties);
    } else {
        drawBackgroundRectangle(painter, adjustedRect(m_menuItemOption->rect), m_backgroundProperties);
    }
}

void MenuItemElement::drawText(QPainter *painter) const
{
    int textFlags = Qt::AlignLeading | Qt::AlignVCenter;
    if (hasText()) {
        AbstractElement::drawText(painter);
    }
    // ShortcutText is just like a regular text element but handled with different name
    // and has different coloration, so override the default colors
    if (!m_shortcutText.isEmpty()) {
        const bool enabled = m_menuItemOption->state.testFlag(QStyle::State_Enabled);
        auto shortcutElements = prepareElements(m_menuItemOption, m_widget, {ElementString::MenuItem, ElementString::ShortcutText});
        const auto properties = queryProperties(shortcutElements);
        auto adjustedOpt = *m_menuItemOption;
        adjustedOpt.rect = adjustedRect(m_menuItemOption->rect).toRect();
        auto map = layoutMap(m_contentElementList, &adjustedOpt, {ElementString::ShortcutText});
        QRectF textRect = map[ElementString::ShortcutText].rect;
        QColor shortcutColor = m_menuItemOption->palette.text().color();
        if (properties->text() && properties->text()->color().has_value()) {
            shortcutColor = properties->text()->color()->toQColor();
        }
        textFlags = textFlagsFromProperties(properties, true);
        painter->save();
        painter->setPen(shortcutColor);
        m_style->drawItemText(painter, textRect.toRect(), textFlags, m_menuItemOption->palette, enabled, m_shortcutText);
        painter->restore();
    }
}

void MenuItemElement::drawIndicator(QPainter *painter) const
{
    if (hasIndicator()) {
        QRectF indicatorRect = m_layoutMap[ElementString::Arrow].rect;
        drawIconAtRect(painter, m_indicator, indicatorRect);
    }
}

QStringList MenuItemElement::elementHints() const
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

qreal MenuItemElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_MenuButtonIndicator:
        return indicatorSize().width();
    default:
        break;
    }
    return 0;
}

QRectF MenuItemElement::adjustedRect(QRectF rect) const
{
    const auto frameWidth = m_style->pixelMetric(QStyle::PM_MenuPanelWidth, m_styleOption, m_widget);
    // Follow what breeze does here to center items. See BreezeStyle::drawMenuItemControl.
    return rect.adjusted(0, 0, -(m_menuHMargin - frameWidth), -(m_menuVMargin - frameWidth));
}

Union::Element::States MenuItemElement::elementStates() const
{
    auto states = AbstractElement::elementStates();
    states.setFlag(Union::Element::State::Checked, m_menuItemOption->checked);
    return states;
}
