/*
 * SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>
#include <QObject>

#include "union_export.h"

namespace Union {

class ElementSelector;
class StyleElement;

class UNION_EXPORT StyleLoader : public QObject
{
    Q_OBJECT

public:
    StyleLoader(QObject* parent = nullptr);
    ~StyleLoader() override;

    virtual bool load(const QUrl &url) = 0;

    virtual std::shared_ptr<StyleElement> get(const ElementSelector &selector) = 0;
};

}
