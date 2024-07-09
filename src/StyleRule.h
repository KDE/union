/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>
#include <optional>

#include <QObject>
#include <QVariant>

#include "Definition.h"
#include "Selector.h"

#include "union_export.h"

namespace Union
{

class StyleRulePrivate;

class StyleRuleInterface
{
public:
    virtual ~StyleRuleInterface()
    {
    }

    virtual QSizeF contentSize() const = 0;
    virtual QRectF boundingRect() const = 0;
    virtual QMarginsF borderSizes() const = 0;

    virtual std::optional<AreaDefinition> foreground() const = 0;
    virtual std::optional<AreaDefinition> background() const = 0;
    virtual std::optional<BorderDefinition> border() const = 0;
    virtual std::optional<CornersDefinition> corners() const = 0;
    virtual std::optional<ShadowDefinition> shadow() const = 0;
    virtual std::optional<BorderDefinition> outset() const = 0;
    virtual std::optional<SizeDefinition> margins() const = 0;
    virtual std::optional<SizeDefinition> padding() const = 0;
    virtual std::optional<TextDefinition> text() const = 0;
};

/**
 * A set of style properties that should be applied to a certain set of elements.
 *
 * This class defines a set of properties to apply to an element, along with a
 * list of selectors that should match for this style to apply.
 */
class UNION_EXPORT StyleRule : public QObject, public StyleRuleInterface, public std::enable_shared_from_this<StyleRule>
{
    Q_OBJECT

public:
    using Ptr = std::shared_ptr<StyleRule>;

    StyleRule(std::unique_ptr<StyleRulePrivate> &&d);
    ~StyleRule() override;

    SelectorList selectors() const;
    void setSelectors(const SelectorList &selectors);

    QSizeF contentSize() const override;
    QRectF boundingRect() const override;
    QMarginsF borderSizes() const override;

    std::optional<AreaDefinition> foreground() const override;
    void setForeground(const std::optional<AreaDefinition> &newForeground);

    std::optional<AreaDefinition> background() const override;
    void setBackground(const std::optional<AreaDefinition> &newBackground);

    std::optional<BorderDefinition> border() const override;
    void setBorder(const std::optional<BorderDefinition> &newBorder);

    std::optional<CornersDefinition> corners() const override;
    void setCorners(const std::optional<CornersDefinition> &newCorners);

    std::optional<ShadowDefinition> shadow() const override;
    void setShadow(const std::optional<ShadowDefinition> &newShadow);

    std::optional<BorderDefinition> outset() const override;
    void setOutset(const std::optional<BorderDefinition> &newOutset);

    std::optional<SizeDefinition> margins() const override;
    void setMargins(const std::optional<SizeDefinition> &newMargin);

    std::optional<SizeDefinition> padding() const override;
    void setPadding(const std::optional<SizeDefinition> &newPadding);

    std::optional<TextDefinition> text() const override;
    void setText(const std::optional<TextDefinition> &newText);

    static Ptr create();

private:
    const std::unique_ptr<StyleRulePrivate> d;
};

}

UNION_EXPORT QDebug operator<<(QDebug debug, std::shared_ptr<Union::StyleRule> style);
