// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off
#include <QObject>
#include <QProperty>
#include <qqmlregistration.h>


#include <properties/SizeProperty.h>

// clang-format on

class SizePropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    SizePropertyGroup();

    void update(const Union::Properties::SizeProperty &newState);

    Q_PROPERTY(qreal left READ left WRITE setLeft BINDABLE bindableLeft NOTIFY leftChanged)
    qreal left() const;
    void setLeft(const qreal &newValue);
    QBindable<qreal> bindableLeft();
    Q_SIGNAL void leftChanged();

    Q_PROPERTY(qreal right READ right WRITE setRight BINDABLE bindableRight NOTIFY rightChanged)
    qreal right() const;
    void setRight(const qreal &newValue);
    QBindable<qreal> bindableRight();
    Q_SIGNAL void rightChanged();

    Q_PROPERTY(qreal top READ top WRITE setTop BINDABLE bindableTop NOTIFY topChanged)
    qreal top() const;
    void setTop(const qreal &newValue);
    QBindable<qreal> bindableTop();
    Q_SIGNAL void topChanged();

    Q_PROPERTY(qreal bottom READ bottom WRITE setBottom BINDABLE bindableBottom NOTIFY bottomChanged)
    qreal bottom() const;
    void setBottom(const qreal &newValue);
    QBindable<qreal> bindableBottom();
    Q_SIGNAL void bottomChanged();

private:
    Q_OBJECT_BINDABLE_PROPERTY(SizePropertyGroup, qreal, m_left, &SizePropertyGroup::leftChanged)
    Q_OBJECT_BINDABLE_PROPERTY(SizePropertyGroup, qreal, m_right, &SizePropertyGroup::rightChanged)
    Q_OBJECT_BINDABLE_PROPERTY(SizePropertyGroup, qreal, m_top, &SizePropertyGroup::topChanged)
    Q_OBJECT_BINDABLE_PROPERTY(SizePropertyGroup, qreal, m_bottom, &SizePropertyGroup::bottomChanged)
};