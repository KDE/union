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

// class ElementSelector;
class ElementIdentifier;
class StyleElement;
class Style;

class UNION_EXPORT StyleLoader : public QObject
{
    Q_OBJECT

public:
    StyleLoader(std::shared_ptr<Style> style, const QString &name, QObject *parent = nullptr);
    ~StyleLoader() override;

    std::shared_ptr<Style> style() const;
    QString name() const;

    virtual bool load() = 0;

private:
    std::shared_ptr<Style> m_style;
    QString m_name;
};

}
