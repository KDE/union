/*
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>

#include <QObject>

#include "Element.h"
#include "Style.h"

#include "union_export.h"

namespace Union
{

class Element;
class Style;

class ThemePrivate;

class UNION_EXPORT Theme : public QObject
{
    Q_OBJECT

public:
    Theme(std::unique_ptr<ThemePrivate> &&d);
    ~Theme() override;

    void load();

    // std::shared_ptr<Element> get(const ElementIdentifier &selector);
    // QList<Style::Ptr> get(const SelectorList &selectors);

    void insert(std::shared_ptr<Style> style);
    QList<Style::Ptr> matches(const QList<Element::Ptr> &elements);

    static std::shared_ptr<Theme> instance();

private:
    const std::unique_ptr<ThemePrivate> d;
};

}
