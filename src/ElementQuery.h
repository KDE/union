/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QList>
#include <QString>

#include "Element.h"
#include "Selector.h"
#include "Style.h"

#include "union_export.h"

namespace Union
{

class Style;
class Theme;

class UNION_EXPORT ElementQuery : public StyleInterface
{
public:
    ElementQuery(std::shared_ptr<Theme> theme);
    ~ElementQuery() override;

    QList<Element::Ptr> elements();
    void setElements(const QList<Element::Ptr> &elements);

    bool execute();

    QSizeF contentSize() const override;
    QRectF boundingRect() const override;
    QMarginsF borderSizes() const override;

    std::optional<AreaDefinition> foreground() const override;
    std::optional<AreaDefinition> background() const override;
    std::optional<BorderDefinition> border() const override;
    std::optional<CornersDefinition> corners() const override;
    std::optional<ShadowDefinition> shadow() const override;
    std::optional<BorderDefinition> outset() const override;
    std::optional<SizeDefinition> margins() const override;
    std::optional<SizeDefinition> padding() const override;

private:
    std::shared_ptr<Theme> m_theme;
    QList<Element::Ptr> m_elements;
    QList<Style::Ptr> m_styles;

    Style::Ptr m_combined;
    // SelectorList m_selectors;
};

}
