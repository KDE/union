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

/**
 * A class encapsulating lookup of style properties based on a list of elements.
 *
 * This class will try find which style rules match a certain list of elements.
 * The list of elements is expected to represent a chain of elements from the
 * root of the application to a specific element.
 *
 * Lookup is done for individual properties, so requesting for example the
 * background color would result in checking if the first matched rule has the
 * background color set and if so, return that. If not the second rule would be
 * checked and so on.
 */
class UNION_EXPORT ElementQuery : public StyleInterface
{
public:
    ElementQuery(std::shared_ptr<Theme> theme = nullptr);
    ~ElementQuery() override;

    QList<Element::Ptr> elements();
    void setElements(const QList<Element::Ptr> &elements);

    void execute();
    bool result() const;

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
    std::optional<TextDefinition> text() const override;

private:
    std::shared_ptr<Theme> m_theme;
    QList<Element::Ptr> m_elements;
    QList<Style::Ptr> m_styles;

    Style::Ptr m_combined;

    bool m_result = false;
};

}
