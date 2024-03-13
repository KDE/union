/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>
#include <optional>

#include <QObject>

#include "Definition.h"

#include "union_export.h"

namespace Union {

class UNION_EXPORT StyleElement : public QObject
{
    Q_OBJECT

public:
    using Ptr = std::shared_ptr<StyleElement>;

    StyleElement(QObject *parent = nullptr);
    ~StyleElement() override;


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

private:
    class Private;
    const std::unique_ptr<Private> d;
};

}
