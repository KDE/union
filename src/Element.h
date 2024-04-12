/*
 * SPDX-FileCopyrightText: 2020 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <memory>

#include <QDebug>
#include <QObject>

#include "Selector.h"

#include "union_export.h"

namespace Union
{

class ElementPrivate;

class UNION_EXPORT Element : public QObject, public std::enable_shared_from_this<Element>
{
    Q_OBJECT
public:
    using Ptr = std::shared_ptr<Element>;

    Element(std::unique_ptr<ElementPrivate> &&dd);
    ~Element() override;

    Q_PROPERTY(QString type READ type WRITE setType BINDABLE bindableType NOTIFY typeChanged)
    QString type() const;
    void setType(const QString &type);
    QBindable<QString> bindableType();
    Q_SIGNAL void typeChanged();

    Q_PROPERTY(QString id READ id WRITE setId BINDABLE bindableId NOTIFY idChanged)
    QString id() const;
    void setId(const QString &newId);
    QBindable<QString> bindableId();
    Q_SIGNAL void idChanged();

    Q_PROPERTY(QStringList states READ states WRITE setStates BINDABLE bindableStates NOTIFY statesChanged)
    QStringList states() const;
    void setStates(const QStringList &newStates);
    QBindable<QStringList> bindableStates();
    Q_SIGNAL void statesChanged();

    Q_PROPERTY(QStringList hints READ hints WRITE setHints BINDABLE bindableHints NOTIFY hintsChanged)
    QStringList hints() const;
    void setHints(const QStringList &newHints);
    QBindable<QStringList> bindableHints();
    Q_SIGNAL void hintsChanged();

    Q_PROPERTY(QVariantMap attributes READ attributes WRITE setAttributes BINDABLE bindableAttributes NOTIFY attributesChanged)
    QVariantMap attributes() const;
    void setAttributes(const QVariantMap &attributes);
    QBindable<QVariantMap> bindableAttributes();
    Q_SIGNAL void attributesChanged();

    bool hasAttribute(const QString &name) const;
    QVariant attribute(const QString &name) const;

    static Element::Ptr create();

private:
    const std::unique_ptr<ElementPrivate> d;
};

using ElementList = QList<Element::Ptr>;

}

UNION_EXPORT QDebug operator<<(QDebug debug, Union::Element::Ptr &element);
