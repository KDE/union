// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#include "ButtonElementV2.h"

#include <PropertiesTypes.h>
#include <StyleRegistry.h>

#include "TextSubElement.h"

using namespace Qt::StringLiterals;
using namespace Union::Widgets;
using namespace Union::Properties;

ButtonElementV2::ButtonElementV2(UnionStyle *style, QWidget *widget)
    : AbstractElementV2(style, widget)
{
}

ButtonElementV2::~ButtonElementV2()
{
}

Union::Element::Ptr ButtonElementV2::element() const
{
    return m_element;
}

void ButtonElementV2::update(const QStyleOption *option)
{
    if (!m_element) {
        createElement(ElementString::Button);
    }

    if (auto buttonOption = qstyleoption_cast<const QStyleOptionButton *>(option); buttonOption) {
        m_element->setHint(u"with-icon"_s, !buttonOption->icon.isNull());
        m_element->setHint(u"flat"_s, buttonOption->features.testFlag(QStyleOptionButton::Flat));
        m_element->setHint(u"with-menu"_s, buttonOption->features.testFlag(QStyleOptionButton::HasMenu));
        m_element->setHint(u"command-link-button"_s, buttonOption->features.testFlag(QStyleOptionButton::CommandLinkButton));

        if (!buttonOption->text.isEmpty()) {
            textSubElement()->setFlag(AbstractElementFlag::DrawMnemonics);
            textSubElement()->setText(buttonOption->text);
            textSubElement()->update(option);
        }
    }

    AbstractElementV2::update(option);
}

void ButtonElementV2::draw(QPainter *painter, DrawVariant what) const
{
    /* clang-format off */
    std::visit(overloaded {
        [](std::nullopt_t) { },
        [this, painter](QStyle::ControlElement element) {
            switch (element) {
            case QStyle::CE_PushButtonBevel:
                drawBackground(painter);
                break;
            case QStyle::CE_PushButtonLabel:
                textSubElement()->draw(painter, std::nullopt);
                break;
            case QStyle::CE_PushButton:
                drawBackground(painter);
                textSubElement()->draw(painter, std::nullopt);
                break;
            default:
                break;
            }
        },
        [](QStyle::ComplexControl) {
        },
        [this, painter](QStyle::PrimitiveElement primitive) {
            switch (primitive) {
            case QStyle::PE_FrameButtonBevel:
            case QStyle::PE_FrameDefaultButton:
                drawFrame(painter);
                break;
            case QStyle::PE_PanelButtonCommand:
            case QStyle::PE_PanelButtonBevel:
                drawBackground(painter);
                break;
            default:
                break;
            }
        }
    }, what);
    /* clang-format on */
}
