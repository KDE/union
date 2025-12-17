// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QIcon>
#include <QQuickItem>

/*!
 * \qmltype Icon
 * \inqmlmodule org.kde.union.impl
 * \ingroup qtquick-core
 *
 * \brief A simple type to display an icon.
 */
class Icon : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    Icon(QQuickItem *parent = nullptr);

    /*!
     * \qmlproperty string Icon::name
     *
     * The name of an icon from the current icon theme to use.
     */
    Q_PROPERTY(QString name READ name WRITE setName RESET resetName NOTIFY nameChanged)
    QString name() const;
    void setName(const QString &newName);
    void resetName();
    Q_SIGNAL void nameChanged();

    /*!
     * \qmlproperty url Icon::source
     */
    Q_PROPERTY(QUrl source READ source WRITE setSource RESET resetSource NOTIFY sourceChanged)
    QUrl source() const;
    void setSource(const QUrl &newSource);
    void resetSource();
    Q_SIGNAL void sourceChanged();

    /*!
     * \qmlproperty color Icon::color
     *
     * A color to use to tint the icon.
     */
    Q_PROPERTY(QColor color READ color WRITE setColor RESET resetColor NOTIFY colorChanged)
    QColor color() const;
    void setColor(const QColor &newColor);
    void resetColor();
    Q_SIGNAL void colorChanged();

    /*!
     * \qmlproperty object Icon::control
     *
     * Convenience property that when set, will automatically bind to the
     * Control's' icon property and update this icon when it changes.
     */
    Q_PROPERTY(QQuickItem *control READ control WRITE setControl RESET resetControl NOTIFY controlChanged)
    QQuickItem *control() const;
    void setControl(QQuickItem *newControl);
    void resetControl();
    Q_SIGNAL void controlChanged();

protected:
    void updatePolish() override;
    QSGNode *updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *) override;

private:
    Q_SLOT void onControlIconChanged();

    QString m_name;
    QUrl m_source;
    QColor m_color = Qt::transparent;
    QIcon m_icon;
    bool m_iconChanged = true;

    QPointer<QQuickItem> m_control;
};
