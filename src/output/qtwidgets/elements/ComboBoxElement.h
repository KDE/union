// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{
class IndicatorSubElement;

class ComboBoxElement : public AbstractElement
{
    Q_OBJECT

public:
    ComboBoxElement(const UnionStyle *style, QWidget *widget);
    // ~ComboBoxElement() override;

    void update(const QStyleOption *option) override;
    void draw(QPainter *painter, DrawVariant what) const override;

    std::optional<QRectF> subControlRect(const QStyleOption *option, QStyle::SubControl subControl) const override;
    QSizeF contentsSize(const QStyleOption *option, QSizeF contentsSizeFromStyle) const override;
    std::optional<qreal> pixelMetric(const QStyleOption *option, QStyle::PixelMetric pixelMetric) const override;

private:
    // bool isEditable() const;
    // Union::IdentifierList elementHints() const override;
    // void updateSubElementList() override;
    // void drawText(QPainter *painter) const override;
    // QString fallbackElementName(const QStyleOption *opt) const override;
    // const QStyleOptionComboBox *m_comboBoxOption = nullptr;
    bool m_editable = false;
    bool m_hasFrame = true;

    IndicatorSubElement *m_indicatorElement = nullptr;

    // Union::ElementList m_indicatorElementList;
    // Union::StylePropertyGroupPtr m_indicatorProperties = nullptr;
};

}
}
