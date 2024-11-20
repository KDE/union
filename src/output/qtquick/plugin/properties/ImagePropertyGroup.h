// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off
#include <QObject>
#include <QProperty>
#include <qqmlregistration.h>


#include <properties/ImageProperty.h>

// clang-format on

class ImagePropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    ImagePropertyGroup();

    void update(const Union::Properties::ImageProperty &newState);
    Q_SIGNAL void updated();

    Q_PROPERTY(QImage imageData READ imageData NOTIFY imageDataChanged)
    QImage imageData() const;
    Q_SIGNAL void imageDataChanged();

    Q_PROPERTY(qreal width READ width NOTIFY widthChanged)
    qreal width() const;
    Q_SIGNAL void widthChanged();

    Q_PROPERTY(qreal height READ height NOTIFY heightChanged)
    qreal height() const;
    Q_SIGNAL void heightChanged();

    Q_PROPERTY(qreal xOffset READ xOffset NOTIFY xOffsetChanged)
    qreal xOffset() const;
    Q_SIGNAL void xOffsetChanged();

    Q_PROPERTY(qreal yOffset READ yOffset NOTIFY yOffsetChanged)
    qreal yOffset() const;
    Q_SIGNAL void yOffsetChanged();

    Q_PROPERTY(Union::Properties::ImageFlags flags READ flags NOTIFY flagsChanged)
    Union::Properties::ImageFlags flags() const;
    Q_SIGNAL void flagsChanged();

private:
    Union::Properties::ImageProperty m_state;
};