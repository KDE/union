// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#include "Icon.h"

#include <QSGImageNode>

#include <KIconColors>

Icon::Icon(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents, true);
}

QString Icon::name() const
{
    return m_name;
}

void Icon::setName(const QString &newName)
{
    if (newName == m_name) {
        return;
    }

    m_name = newName;
    polish();
    Q_EMIT nameChanged();
}

void Icon::resetName()
{
    setName(QString{});
}

QColor Icon::color() const
{
    return m_color;
}

void Icon::setColor(const QColor &newColor)
{
    if (newColor == m_color) {
        return;
    }

    m_color = newColor;
    polish();
    Q_EMIT colorChanged();
}

void Icon::resetColor()
{
    setColor(Qt::transparent);
}

QQuickItem *Icon::control() const
{
    return m_control;
}

void Icon::setControl(QQuickItem *newControl)
{
    if (newControl == m_control) {
        return;
    }

    if (m_control) {
        m_control->disconnect(this);
    }

    m_control = newControl;

    if (m_control) {
        connect(m_control, SIGNAL(iconChanged()), this, SLOT(onControlIconChanged()));

        onControlIconChanged();
    }

    Q_EMIT controlChanged();
}

void Icon::resetControl()
{
    setControl(nullptr);
}

QSGNode *Icon::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *)
{
    if (m_icon.isNull()) {
        return nullptr;
    }

    if (!node) {
        node = window()->createImageNode();
    }

    auto imageNode = static_cast<QSGImageNode *>(node);

    auto bounds = boundingRect();

    imageNode->setRect(bounds);
    imageNode->setTexture(window()->createTextureFromImage(m_icon.pixmap(bounds.size().toSize()).toImage(), QQuickWindow::TextureCanUseAtlas));
    imageNode->setOwnsTexture(true);
    imageNode->setFiltering(QSGTexture::Linear);

    return node;
}

void Icon::updatePolish()
{
    // TODO Move this into some sort of platform plugin
#ifdef WITH_KICONTHEMES
    if (m_color != Qt::transparent) {
        KIconColors colors;
        colors.setText(m_color);
        m_icon = KDE::icon(m_name, colors);
    } else {
        m_icon = KDE::icon(m_name);
    }
#else
    m_icon = QIcon::fromTheme(m_name);
#endif

    update();
}

void Icon::onControlIconChanged()
{
    static auto iconType = QMetaType::fromName("QQuickIcon").metaObject();
    static auto widthProperty = iconType->property(iconType->indexOfProperty("width"));
    static auto heightProperty = iconType->property(iconType->indexOfProperty("height"));
    static auto nameProperty = iconType->property(iconType->indexOfProperty("name"));
    static auto colorProperty = iconType->property(iconType->indexOfProperty("color"));

    auto icon = m_control->property("icon");
    if (!icon.isValid()) {
        return;
    }

    setImplicitSize(widthProperty.readOnGadget(icon.constData()).toReal(), heightProperty.readOnGadget(icon.constData()).toReal());
    setName(nameProperty.readOnGadget(icon.constData()).toString());
    setColor(colorProperty.readOnGadget(icon.constData()).value<QColor>());
}
