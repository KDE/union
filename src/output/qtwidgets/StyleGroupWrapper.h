// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include "StyleUtils.h"

namespace Union
{
namespace Widgets
{

class StyleGroupWrapper
{
public:
    StyleGroupWrapper();
    StyleGroupWrapper(const StylePropertyGroupPtr &propertyGroup);

    StylePropertyGroupPtr properties() const;

    QSizeF adjustSize(QSizeF contentsSize, bool includesAveragePadding) const;

    QSizeF size() const;
    qreal width() const;
    qreal height() const;

    QMarginsF padding() const;
    qreal averagePadding() const;
    qreal averageHPadding() const;
    qreal averageVPadding() const;

    qreal spacing() const;

    QMarginsF borderSize() const;
    qreal averageBorderSize() const;

    QSizeF iconSize() const;

    int textFlags(Qt::LayoutDirection direction) const;
    // QString elidedText(const QString &text, const QRectF &textRect, ) const;

    /*!
     * \brief Updates the outline and dropshadow for the element,
     * if it has the graphics widget added.
     */
    // void updateGraphicsEffect(const QRectF &rect, const Union::StylePropertyGroupPtr &properties = nullptr) const;

    /*!
     * \brief Helper method for returning visualRect from style(),
     * using styleoption direction.
     */
    // QRectF visualRect(const QRectF &rect) const;

private:
    Union::StylePropertyGroupPtr m_properties = nullptr;
};

}
}
