/**
 * SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QMarginsF>
#include <qqmlregistration.h>

#include <Definition.h>
#include <Element.h>

namespace ColorSet
{
Q_NAMESPACE
QML_ELEMENT
QML_FOREIGN_NAMESPACE(Union::Element)
}

class Sizes
{
    Q_GADGET
    QML_VALUE_TYPE(sizes)

public:
    Sizes();
    Sizes(qreal left, qreal right, qreal top, qreal bottom);
    explicit Sizes(const QMarginsF &margins);
    explicit Sizes(const std::optional<Union::SizeDefinition> &other);

    bool operator==(const Sizes &other);

    Q_PROPERTY(qreal left READ left WRITE setLeft)
    qreal left() const;
    void setLeft(qreal newLeft);

    Q_PROPERTY(qreal right READ right WRITE setRight)
    qreal right() const;
    void setRight(qreal newRight);

    Q_PROPERTY(qreal top READ top WRITE setTop)
    qreal top() const;
    void setTop(qreal newTop);

    Q_PROPERTY(qreal bottom READ bottom WRITE setBottom)
    qreal bottom() const;
    void setBottom(qreal newBottom);

    Q_PROPERTY(bool valid READ isValid)
    bool isValid() const;

    QMarginsF toMargins() const;

private:
    QMarginsF m_margins;
};
