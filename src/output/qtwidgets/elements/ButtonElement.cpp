// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ButtonElement.h"
#include "SharedNames.h"
#include "StyleUtils.h"
#include "UnionStyle.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyle>

#include "PropertiesTypes.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;

ButtonElement::ButtonElement(const QStyleOptionButton *option, const UnionStyle *style, const QWidget *widget)
    : AbstractElement(option, style, widget)
    , m_buttonOption(option)
{
    update();
}

ButtonElement::~ButtonElement()
{
}

void ButtonElement::update()
{
    setIndicator(QIcon());
    if (m_buttonOption->features.testFlag(QStyleOptionButton::HasMenu)) {
        m_indicatorElementList = prepareElements(m_styleOption, m_widget, {ElementString::Indicator});
        if (!m_indicatorElementList.isEmpty()) {
            m_indicatorProperties = queryProperties(m_indicatorElementList);
            if (m_indicatorProperties->icon()) {
                setIndicator(m_style->unionIcon(m_indicatorProperties, QString()));
            }
        }
    }

    setIcon(m_buttonOption->icon);
    setText(m_buttonOption->text);

    updateSubElementList();
    layout();
}

void ButtonElement::draw(QPainter *painter, DrawEnums enums) const
{
    if (!m_isValid) {
        return;
    }
    switch (enums.ControlElement) {
    case QStyle::CE_PushButtonBevel:
        drawBackground(painter);
        break;
    case QStyle::CE_PushButtonLabel:
        drawIcon(painter);
        drawText(painter);
        break;
    case QStyle::CE_PushButton:
        drawBackground(painter);
        drawIcon(painter);
        drawText(painter);
        drawIndicator(painter);
        break;
    }
    switch (enums.PrimitiveElement) {
    case QStyle::PE_FrameButtonBevel:
    case QStyle::PE_FrameDefaultButton:
        drawFrame(painter);
        break;
    case QStyle::PE_PanelButtonCommand:
    case QStyle::PE_PanelButtonBevel:
        drawBackground(painter);
        break;
    }
}

void ButtonElement::updateSubElementList()
{
    m_subElementList.clear();
    if (m_buttonOption) {
        if (m_buttonOption->features.testFlag(QStyleOptionButton::HasMenu)) {
            m_subElementList.append(ElementString::Indicator);
        }
        if (!m_buttonOption->icon.isNull()) {
            m_subElementList.append(ElementString::Icon);
        }
        if (!m_buttonOption->text.isEmpty()) {
            m_subElementList.append(ElementString::Text);
        }
    }
}

QSizeF ButtonElement::contentsSize(const QSizeF &contentsSizeFromStyle) const
{
    Q_UNUSED(contentsSizeFromStyle);
    QSizeF size = subElementRect(QStyle::SE_PushButtonContents).size();
    size = applyPaddingToSize(size);
    // Since text and icon are parts of background, we need to apply the indicator width and spacing from background
    // to get the proper contentSize
    if (hasIndicator()) {
        qreal spacing = m_backgroundProperties->safePropertyLookup(0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::spacing);
        size.rwidth() += m_layoutMap[ElementString::Indicator].rect.width() + spacing;
    }
    return size;
}

QRectF ButtonElement::subElementRect(QStyle::SubElement element) const
{
    if (!m_isValid) {
        qCWarning(UNION_QTWIDGETS) << "Subelementrect for " << element << "is not valid";
        return QRect();
    }

    if (element == QStyle::SE_PushButtonBevel || element == QStyle::SE_PushButtonFocusRect) {
        return backgroundRectangle(m_buttonOption, m_backgroundProperties).toRect();
    }

    return unifiedRect(m_layoutMap);
}

Union::Element::States ButtonElement::elementStates() const
{
    auto states = AbstractElement::elementStates();
    if (m_buttonOption->features.testFlag(QStyleOptionButton::ButtonFeature::DefaultButton)
        || m_buttonOption->features.testFlag(QStyleOptionButton::ButtonFeature::AutoDefaultButton)) {
        states.setFlag(Union::Element::State::Highlighted, true);
    }
    return states;
}

QStringList ButtonElement::elementHints() const
{
    QStringList hints;
    if (m_buttonOption->features.testFlag(QStyleOptionButton::ButtonFeature::Flat)) {
        hints.append(u"flat"_s);
    }
    if (m_buttonOption->features.testFlag(QStyleOptionButton::ButtonFeature::HasMenu)) {
        hints.append(u"with-menu"_s);
    }
    if (m_buttonOption->features.testFlag(QStyleOptionButton::ButtonFeature::CommandLinkButton)) {
        hints.append(u"command-link-button"_s);
    }
    return hints;
}

qreal ButtonElement::pixelMetric(QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_ButtonMargin:
        return averagePadding();
    case QStyle::PM_ButtonDefaultIndicator:
        return averageBorderSize();
    case QStyle::PM_ButtonIconSize:
        return iconSize().width();
    default:
        break;
    }
    return 0;
}
