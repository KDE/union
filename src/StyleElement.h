/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>
#include <optional>

#include <QObject>

#include "union_export.h"

namespace Union {

class UNION_EXPORT StyleElement : public QObject
{
    Q_OBJECT

public:
    using Ptr = std::shared_ptr<StyleElement>;

    StyleElement(QObject *parent = nullptr);
    ~StyleElement() override;

//     Q_PROPERTY(VisualDefinition *foreground READ foreground NOTIFY foregroundChanged)
//     VisualDefinition

//     Q_PROPERTY(VisualDefinition *background READ background NOTIFY backgroundChanged)

private:
    class Private;
    const std::unique_ptr<Private> d;
};

}
