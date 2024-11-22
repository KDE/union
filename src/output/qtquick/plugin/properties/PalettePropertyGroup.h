// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
//
// This file is automatically generated. To regenerate, run `tools/propertygenerator/generate_properties.py`.

#pragma once

// clang-format off
#include <QJSValue>
#include <QObject>
#include <QProperty>
#include <qqmlregistration.h>

#include <QGuiApplication>
#include <QQmlComponent>
#include <QVariant>
#include <QWindow>

#include <properties/PaletteProperty.h>

// clang-format on

class QuickStyle;

class PalettePropertyGroup : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit PalettePropertyGroup(QuickStyle *style);

    void update(const Union::Properties::PaletteProperty &newState);
    Q_SIGNAL void updated();

    Q_PROPERTY(QJSValue accent READ accent NOTIFY accentChanged)
    QJSValue accent() const;
    Q_SIGNAL void accentChanged();

    Q_PROPERTY(QJSValue alternateBase READ alternateBase NOTIFY alternateBaseChanged)
    QJSValue alternateBase() const;
    Q_SIGNAL void alternateBaseChanged();

    Q_PROPERTY(QJSValue base READ base NOTIFY baseChanged)
    QJSValue base() const;
    Q_SIGNAL void baseChanged();

    Q_PROPERTY(QJSValue brightText READ brightText NOTIFY brightTextChanged)
    QJSValue brightText() const;
    Q_SIGNAL void brightTextChanged();

    Q_PROPERTY(QJSValue button READ button NOTIFY buttonChanged)
    QJSValue button() const;
    Q_SIGNAL void buttonChanged();

    Q_PROPERTY(QJSValue buttonText READ buttonText NOTIFY buttonTextChanged)
    QJSValue buttonText() const;
    Q_SIGNAL void buttonTextChanged();

    Q_PROPERTY(QJSValue dark READ dark NOTIFY darkChanged)
    QJSValue dark() const;
    Q_SIGNAL void darkChanged();

    Q_PROPERTY(QJSValue highlight READ highlight NOTIFY highlightChanged)
    QJSValue highlight() const;
    Q_SIGNAL void highlightChanged();

    Q_PROPERTY(QJSValue highlightedText READ highlightedText NOTIFY highlightedTextChanged)
    QJSValue highlightedText() const;
    Q_SIGNAL void highlightedTextChanged();

    Q_PROPERTY(QJSValue light READ light NOTIFY lightChanged)
    QJSValue light() const;
    Q_SIGNAL void lightChanged();

    Q_PROPERTY(QJSValue link READ link NOTIFY linkChanged)
    QJSValue link() const;
    Q_SIGNAL void linkChanged();

    Q_PROPERTY(QJSValue linkVisited READ linkVisited NOTIFY linkVisitedChanged)
    QJSValue linkVisited() const;
    Q_SIGNAL void linkVisitedChanged();

    Q_PROPERTY(QJSValue mid READ mid NOTIFY midChanged)
    QJSValue mid() const;
    Q_SIGNAL void midChanged();

    Q_PROPERTY(QJSValue midlight READ midlight NOTIFY midlightChanged)
    QJSValue midlight() const;
    Q_SIGNAL void midlightChanged();

    Q_PROPERTY(QJSValue placeholderText READ placeholderText NOTIFY placeholderTextChanged)
    QJSValue placeholderText() const;
    Q_SIGNAL void placeholderTextChanged();

    Q_PROPERTY(QJSValue shadow READ shadow NOTIFY shadowChanged)
    QJSValue shadow() const;
    Q_SIGNAL void shadowChanged();

    Q_PROPERTY(QJSValue text READ text NOTIFY textChanged)
    QJSValue text() const;
    Q_SIGNAL void textChanged();

    Q_PROPERTY(QJSValue toolTipBase READ toolTipBase NOTIFY toolTipBaseChanged)
    QJSValue toolTipBase() const;
    Q_SIGNAL void toolTipBaseChanged();

    Q_PROPERTY(QJSValue toolTipText READ toolTipText NOTIFY toolTipTextChanged)
    QJSValue toolTipText() const;
    Q_SIGNAL void toolTipTextChanged();

    Q_PROPERTY(QJSValue window READ window NOTIFY windowChanged)
    QJSValue window() const;
    Q_SIGNAL void windowChanged();

    Q_PROPERTY(QJSValue windowText READ windowText NOTIFY windowTextChanged)
    QJSValue windowText() const;
    Q_SIGNAL void windowTextChanged();

    Q_PROPERTY(QJSValue positive READ positive NOTIFY positiveChanged)
    QJSValue positive() const;
    Q_SIGNAL void positiveChanged();

    Q_PROPERTY(QJSValue neutral READ neutral NOTIFY neutralChanged)
    QJSValue neutral() const;
    Q_SIGNAL void neutralChanged();

    Q_PROPERTY(QJSValue negative READ negative NOTIFY negativeChanged)
    QJSValue negative() const;
    Q_SIGNAL void negativeChanged();

    Q_PROPERTY(QObject *quickPalette READ quickPalette NOTIFY updated)
    QObject *quickPalette() const;

private:
    QuickStyle *m_style = nullptr;
    Union::Properties::PaletteProperty m_state;
    QObject *m_palette = nullptr;
    QQmlComponent *m_component = nullptr;
};