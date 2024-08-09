// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `scripts/generate_properties.py`.

#pragma once

// clang-format off
#include <QObject>
#include <QProperty>
#include <qqmlregistration.h>

#include <QImage>

#include <properties/ImageProperty.h>

// clang-format on

class ImagePropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    ImagePropertyGroup();

    void update(const Union::Properties::ImageProperty &newState);

    Q_PROPERTY(QImage imageData READ imageData WRITE setImageData BINDABLE bindableImageData NOTIFY imageDataChanged)
    QImage imageData() const;
    void setImageData(const QImage &newValue);
    QBindable<QImage> bindableImageData();
    Q_SIGNAL void imageDataChanged();

    Q_PROPERTY(qreal width READ width WRITE setWidth BINDABLE bindableWidth NOTIFY widthChanged)
    qreal width() const;
    void setWidth(const qreal &newValue);
    QBindable<qreal> bindableWidth();
    Q_SIGNAL void widthChanged();

    Q_PROPERTY(qreal height READ height WRITE setHeight BINDABLE bindableHeight NOTIFY heightChanged)
    qreal height() const;
    void setHeight(const qreal &newValue);
    QBindable<qreal> bindableHeight();
    Q_SIGNAL void heightChanged();

    Q_PROPERTY(qreal xOffset READ xOffset WRITE setXOffset BINDABLE bindableXOffset NOTIFY xOffsetChanged)
    qreal xOffset() const;
    void setXOffset(const qreal &newValue);
    QBindable<qreal> bindableXOffset();
    Q_SIGNAL void xOffsetChanged();

    Q_PROPERTY(qreal yOffset READ yOffset WRITE setYOffset BINDABLE bindableYOffset NOTIFY yOffsetChanged)
    qreal yOffset() const;
    void setYOffset(const qreal &newValue);
    QBindable<qreal> bindableYOffset();
    Q_SIGNAL void yOffsetChanged();

    Q_PROPERTY(Union::Properties::ImageFlags flags READ flags WRITE setFlags BINDABLE bindableFlags NOTIFY flagsChanged)
    Union::Properties::ImageFlags flags() const;
    void setFlags(const Union::Properties::ImageFlags &newValue);
    QBindable<Union::Properties::ImageFlags> bindableFlags();
    Q_SIGNAL void flagsChanged();

private:
    Q_OBJECT_BINDABLE_PROPERTY(ImagePropertyGroup, QImage, m_imageData, &ImagePropertyGroup::imageDataChanged)
    Q_OBJECT_BINDABLE_PROPERTY(ImagePropertyGroup, qreal, m_width, &ImagePropertyGroup::widthChanged)
    Q_OBJECT_BINDABLE_PROPERTY(ImagePropertyGroup, qreal, m_height, &ImagePropertyGroup::heightChanged)
    Q_OBJECT_BINDABLE_PROPERTY(ImagePropertyGroup, qreal, m_xOffset, &ImagePropertyGroup::xOffsetChanged)
    Q_OBJECT_BINDABLE_PROPERTY(ImagePropertyGroup, qreal, m_yOffset, &ImagePropertyGroup::yOffsetChanged)
    Q_OBJECT_BINDABLE_PROPERTY(ImagePropertyGroup, Union::Properties::ImageFlags, m_flags, &ImagePropertyGroup::flagsChanged)
};