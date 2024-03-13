/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 * 
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>
#include <QObject>

#include "union_export.h"

namespace Union {

class StyleElement;
class ElementIdentifier;

class UNION_EXPORT Style : public QObject
{
    Q_OBJECT

public:
    Style();
    ~Style() override;

    void load();

    std::shared_ptr<StyleElement> get(const ElementIdentifier &selector);

    void insert(const ElementIdentifier &identifier, std::shared_ptr<StyleElement> element);

    static std::shared_ptr<Style> instance();

private:
    class Private;
    const std::unique_ptr<Private> d;
};

}
