// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@quantumproductions.info>

#include "Icon.h"

#include <QQmlProperty>
#include <QSGImageNode>

#include <StyleRegistry.h>

#include "qtquick_logging.h"

using namespace Union::Quick;
using namespace Qt::StringLiterals;

// Get the smallest of the icon's availableSizes that fits in size
QSize iconSizeForSize(const QIcon &icon, const QSize &size)
{
    auto availableSizes = icon.availableSizes();
    std::ranges::sort(availableSizes, [](const QSize &first, const QSize &second) {
        return first.width() < second.width();
    });
    auto smallest = availableSizes.end();
    for (auto itr = availableSizes.begin(); itr != availableSizes.end(); ++itr) {
        if (itr->width() > size.width() || itr->height() > size.height()) {
            if (itr != availableSizes.begin()) {
                smallest = itr - 1;
            } else {
                smallest = itr;
            }
            break;
        }
    }

    if (smallest == availableSizes.end()) {
        return QSize{};
    } else {
        return *smallest;
    }
}

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

QUrl Icon::source() const
{
    return m_source;
}

void Icon::setSource(const QUrl &newSource)
{
    if (newSource == m_source) {
        return;
    }

    if (!newSource.isLocalFile()) {
        if (newSource.isRelative()) {
            // Some code might incorrectly set icon.source to an icon name, in that case
            // treat the URL as a name.
            qCWarning(UNION_QTQUICK) << "Relative URL used as Icon source, this is wrong, interpreting it as icon name instead";
            setName(newSource.toString());
        } else {
            qCDebug(UNION_QTQUICK) << "Non-local source URL" << newSource << "used as Icon source is unsupported";
        }
    }

    m_source = newSource;
    Q_EMIT sourceChanged();
}

void Icon::resetSource()
{
    setSource(QUrl{});
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
        QQmlProperty iconProperty(m_control, u"icon"_s);
        iconProperty.connectNotifySignal(this, SLOT(onControlIconChanged()));
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

    imageNode->setRect(QRectF{std::round(bounds.x() + (bounds.width() - m_iconSize.width()) / 2.0),
                              std::round(bounds.y() + (bounds.height() - m_iconSize.height()) / 2.0),
                              qreal(m_iconSize.width()),
                              qreal(m_iconSize.height())});
    imageNode->setOwnsTexture(true);
    imageNode->setFiltering(QSGTexture::Linear);

    if (m_iconChanged || !imageNode->texture()) {
        const auto mode = isEnabled() ? QIcon::Mode::Normal : QIcon::Mode::Disabled;
        auto image = m_icon.pixmap(m_iconSize, window()->devicePixelRatio(), mode).toImage();
        imageNode->setTexture(window()->createTextureFromImage(image, QQuickWindow::TextureCanUseAtlas));
        m_iconChanged = false;
    }

    return node;
}

void Icon::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    if (change == QQuickItem::ItemChange::ItemEnabledHasChanged) {
        m_iconChanged = true;
        update();
    }

    QQuickItem::itemChange(change, value);
}

void Icon::geometryChange(const QRectF &newGeometry, [[maybe_unused]] const QRectF &oldGeometry)
{
    m_iconSize = iconSizeForSize(m_icon, newGeometry.size().toSize());
    update();
}

void Icon::updatePolish()
{
    if (!m_source.isEmpty() && m_source.isLocalFile()) {
        m_icon = QIcon(m_source.toLocalFile());
    } else {
        m_icon = Union::StyleRegistry::instance()->platform()->platformIcon(m_name, m_color);
    }

    m_iconSize = iconSizeForSize(m_icon, boundingRect().size().toSize());
    m_iconChanged = true;
    update();
}

void Icon::onControlIconChanged()
{
    static auto iconType = QMetaType::fromName("QQuickIcon").metaObject();
    static auto widthProperty = iconType->property(iconType->indexOfProperty("width"));
    static auto heightProperty = iconType->property(iconType->indexOfProperty("height"));
    static auto nameProperty = iconType->property(iconType->indexOfProperty("name"));
    static auto sourceProperty = iconType->property(iconType->indexOfProperty("source"));
    static auto colorProperty = iconType->property(iconType->indexOfProperty("color"));

    auto icon = m_control->property("icon");
    if (!icon.isValid()) {
        return;
    }

    auto data = icon.constData();
    setImplicitSize(widthProperty.readOnGadget(data).toReal(), heightProperty.readOnGadget(data).toReal());
    setName(nameProperty.readOnGadget(data).toString());
    setSource(sourceProperty.readOnGadget(data).toUrl());
    setColor(colorProperty.readOnGadget(data).value<QColor>());
}

#include "moc_Icon.cpp"
