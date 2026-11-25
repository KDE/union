// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "DockWidgetElement.h"

#include <QDockWidget>

#include "TextSubElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

static OptionToElementRegistration<DockWidgetElement, QStyleOption::SO_DockWidget> dockOptionRegistration;
static WidgetToElementRegistration<DockWidgetElement, QDockWidget> dockWidgetRegistration;

DockWidgetElement::DockWidgetElement(const UnionStyle *style, QWidget *widget)
    : AbstractElement(style, widget)
{
    // setFlag(AbstractElementFlag::DrawMnemonics);
    // update(option);
}

void DockWidgetElement::update(const QStyleOption *option)
{
    auto e = ensureElement();
    e->setType(ElementString::DockWidget);

    const auto dockOption = qstyleoption_cast<const QStyleOptionDockWidget *>(option);
    if (dockOption) {
        if (!dockOption->title.isEmpty()) {
            firstChildOfType<TextSubElement>(true)->setText(dockOption->title);
        } else {
            removeChildElement(firstChildOfType<TextSubElement>());
        }

        if (dockOption->closable) {
            if (!m_closeIndicator) {
                m_closeIndicator = new IndicatorSubElement(style(), widget(), {ElementString::CloseButton});
                addChildElement(m_closeIndicator);
            }
        } else {
            if (m_closeIndicator) {
                removeChildElement(m_closeIndicator);
                m_closeIndicator = nullptr;
            }
        }

        if (dockOption->floatable) {
            if (!m_floatIndicator) {
                m_floatIndicator = new IndicatorSubElement(style(), widget(), {ElementString::FloatButton});
                addChildElement(m_floatIndicator);
            }
        } else {
            if (m_floatIndicator) {
                removeChildElement(m_floatIndicator);
                m_floatIndicator = nullptr;
            }
        }

        // m_closable =
        // setIcon(QIcon());
        // setText(m_dockWidgetOption->title);
        // updateSubElementList();
    }
    // layout();
    AbstractElement::update(option);
}

void DockWidgetElement::draw(QPainter *painter, DrawVariant what) const
{
    // if (!m_isValid) {
    //     return;
    // }

    if (std::holds_alternative<QStyle::ControlElement>(what)) {
        if (std::get<QStyle::ControlElement>(what) == QStyle::CE_DockWidgetTitle) {
            drawBackground(painter);
            drawChildren<TextSubElement>(painter);
        }
    }
}

// void DockWidgetElement::updateSubElementList()
// {
//     m_subElementList.clear();
//     if (m_dockWidgetOption) {
//         if (m_dockWidgetOption->closable) {
//             m_subElementList.append(ElementString::CloseButton);
//         }
//         if (m_dockWidgetOption->floatable) {
//             m_subElementList.append(ElementString::FloatButton);
//         }
//         if (!m_dockWidgetOption->title.isEmpty()) {
//             m_subElementList.append(ElementString::Text);
//         }
//     }
// }

// void DockWidgetElement::layout()
// {
//     m_elementList = prepareElements(option, widget(), {ElementString::DockWidget});
//     if (!m_elementList.isEmpty()) {
//         m_elementProperties = queryProperties(m_elementList);
//         m_layoutMap = layoutMap(option, m_subElementList);
//         m_isValid = true;
//     } else {
//         m_isValid = false;
//         qCWarning(UNION_QTWIDGETS) << "Could not find elementlist for this element!";
//     }
// }

QRectF DockWidgetElement::subElementRect(const QStyleOption *option, QStyle::SubElement subElement) const
{
    if (subElement == QStyle::SE_DockWidgetTitleBarText) {
        return visualRect(firstChildOfType<TextSubElement>());
    } else if (subElement == QStyle::SE_DockWidgetFloatButton) {
        return visualRect(m_floatIndicator);
    } else if (subElement == QStyle::SE_DockWidgetCloseButton) {
        return visualRect(m_closeIndicator);
    }
    // else if (subElement == QStyle::SE_DockWidgetIcon) {
    //    // The styleoption has no icon, yet there is whole thing for an icon? Wtf.
    //    // return visualRect(m_layoutMap[ElementString::Icon].rect);
    // }

    return style()->QCommonStyle::subElementRect(subElement, option, widget());
}

qreal DockWidgetElement::pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const
{
    switch (pixelMetric) {
    case QStyle::PM_DockWidgetFrameWidth:
        return propertyWrapper().averageBorderSize();
    default:
        break;
    }
    return 0;
}

QIcon DockWidgetElement::standardPixmap(const QStyleOption *option, QStyle::StandardPixmap pixmap) const
{
    switch (pixmap) {
    case QStyle::SP_TitleBarNormalButton:
        if (m_floatIndicator) {
            return style()->unionIcon(m_floatIndicator->properties(), u"window-restore"_s);
        }
        break;
    case QStyle::SP_TitleBarCloseButton:
    case QStyle::SP_DockWidgetCloseButton:
        if (m_closeIndicator) {
            return style()->unionIcon(m_closeIndicator->properties(), u"window-close-symbolic"_s);
        }
        break;
    default:
        break;
    }

    return style()->QCommonStyle::standardPixmap(pixmap, option, widget());
}

// QString DockWidgetElement::fallbackElementName([[maybe_unused]] const QStyleOption *opt) const
// {
//     return ElementString::DockWidget;
// }
