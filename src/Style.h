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

namespace Union {

class StylePrivate;

class UNION_EXPORT Style : public QObject, public std::enable_shared_from_this<Style>
{
    Q_OBJECT

public:
    using Ptr = std::shared_ptr<Style>;

    Style(std::unique_ptr<StylePrivate> &&d);
    ~Style() override;

    SelectorList selectors() const;
    void setSelectors(const SelectorList &selectors);

    QSizeF contentSize() const;
    QRectF boundingRect() const;
    QMarginsF borderSizes() const;

    std::optional<AreaDefinition> foreground() const;
    void setForeground(const std::optional<AreaDefinition> &newForeground);

    std::optional<AreaDefinition> background() const;
    void setBackground(const std::optional<AreaDefinition> &newBackground);

    std::optional<BorderDefinition> border() const;
    void setBorder(const std::optional<BorderDefinition> &newBorder);

    std::optional<ShadowDefinition> shadow() const;
    void setShadow(const std::optional<ShadowDefinition> &newShadow);

    std::optional<SizeDefinition> margin() const;
    void setMargin(const std::optional<SizeDefinition> &newMargin);

    std::optional<SizeDefinition> padding() const;
    void setPadding(const std::optional<SizeDefinition> &newPadding);

    std::optional<BorderDefinition> outset() const;
    void setOutset(const std::optional<BorderDefinition> &newOutset);

    std::optional<CornersDefinition> corners() const;
    void setCorners(const std::optional<CornersDefinition> &newCorners);

    static Style::Ptr create();

private:
    const std::unique_ptr<StylePrivate> d;
};

}

UNION_EXPORT QDebug operator<<(QDebug debug, std::shared_ptr<Union::Style> style);
