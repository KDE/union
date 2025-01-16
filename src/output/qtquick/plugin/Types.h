// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QMarginsF>
#include <qqmlregistration.h>

#include <Element.h>
#include <PropertiesTypes.h>

namespace ColorSet
{
Q_NAMESPACE
QML_ELEMENT
QML_FOREIGN_NAMESPACE(Union::Element)
}

class Alignment : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    using QObject::QObject;

    enum AlignmentType {
        Unspecified = int(Union::Properties::Alignment::Unspecified),
        Start = int(Union::Properties::Alignment::Start),
        Center = int(Union::Properties::Alignment::Center),
        End = int(Union::Properties::Alignment::End),
        Fill = int(Union::Properties::Alignment::Fill),
        Stack = int(Union::Properties::Alignment::Stack),
    };
    Q_ENUM(AlignmentType)

    Q_INVOKABLE Qt::Alignment toQtHorizontal(AlignmentType alignment);
    Q_INVOKABLE Qt::Alignment toQtVertical(AlignmentType alignment);
};

/**
 * A helper type that wraps and exposes a QMarginsF to QML.
 */
class Sizes
{
    Q_GADGET
    QML_VALUE_TYPE(sizes)

public:
    Sizes();
    Sizes(qreal left, qreal right, qreal top, qreal bottom);
    explicit Sizes(const QMarginsF &margins);

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
    friend bool operator==(const Sizes &, const Sizes &);
    QMarginsF m_margins;
};

bool operator==(const Sizes &first, const Sizes &second);
