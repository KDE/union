/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>
#include <QObject>

#include "Style.h"
#include "union_export.h"

namespace Union {

class Element;
class Style;
class Theme;

class UNION_EXPORT StyleLoader : public QObject
{
    Q_OBJECT

public:
    StyleLoader(std::shared_ptr<Theme> theme, const QString &name, QObject *parent = nullptr);
    ~StyleLoader() override;

    std::shared_ptr<Theme> theme() const;
    QString name() const;

    virtual bool load() = 0;

private:
    std::shared_ptr<Theme> m_theme;
    QString m_name;
};

}
