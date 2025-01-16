// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <memory>

#include <QObject>

#include "Element.h"
#include "StyleRule.h"

#include "union_export.h"

namespace Union
{

class ThemeLoader;
class ThemePrivate;

class UNION_EXPORT Theme : public QObject, public std::enable_shared_from_this<Theme>
{
    Q_OBJECT

public:
    using Ptr = std::shared_ptr<Theme>;

    Theme(std::unique_ptr<ThemePrivate> &&d);
    ~Theme() override;

    Q_PROPERTY(QString name READ name CONSTANT)
    QString name() const;

    Q_PROPERTY(QString pluginName READ pluginName CONSTANT)
    QString pluginName() const;

    ThemeLoader *loader() const;
    bool load();

    QList<StyleRule::Ptr> matches(const QList<Element::Ptr> &elements);

    void insert(StyleRule::Ptr style);
    QList<StyleRule::Ptr> rules();

    static Ptr create(const QString &pluginName, const QString &themeName, std::unique_ptr<ThemeLoader> &&loader);

private:
    const std::unique_ptr<ThemePrivate> d;
};

}
