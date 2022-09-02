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

class UNION_EXPORT ElementSelector
{
public:
    ElementSelector();
    ElementSelector(const ElementSelector &other);
    ElementSelector(ElementSelector &&other);

    ~ElementSelector();

    ElementSelector& operator=(const ElementSelector &other);
    ElementSelector& operator=(ElementSelector &&other);

    QString elementName() const;
    void setElementName(const QString &name);

private:
    class Private;
    std::unique_ptr<Private> d;
};

}

