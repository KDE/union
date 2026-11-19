// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "StyleGroupWrapper.h"

#include "UnionStyle.h"

using namespace Qt::StringLiterals;
using namespace Union::Properties;
using namespace Union::Widgets;

StyleGroupWrapper::StyleGroupWrapper()
{
}

StyleGroupWrapper::StyleGroupWrapper(const StylePropertyGroupPtr &propertyGroup)
    : m_properties(propertyGroup)
{
}

Union::StylePropertyGroupPtr StyleGroupWrapper::properties() const
{
    return m_properties;
}

QSizeF StyleGroupWrapper::adjustSize(QSizeF contentsSize, bool includesAveragePadding) const
{
    if (!m_properties) {
        return contentsSize;
    }

    QMarginsF padding =
        safePropertyLookup(m_properties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
    QMarginsF inset = safePropertyLookup(m_properties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::inset, &SizePropertyGroup::toMargins);

    auto width = safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width);
    auto height = safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height);
    auto preferredSize = QSizeF(width, height);

    auto size = contentsSize;
    if (includesAveragePadding) {
        size.rwidth() -= averageHPadding() * 2;
        size.rheight() -= averageVPadding() * 2;
    }

    // We need to apply the maximum of each component of either padding or inset
    // since we need to ensure things are large enough for either of these
    // properties.
    auto spacing = QMarginsF{
        std::max(padding.left(), inset.left()),
        std::max(padding.top(), inset.top()),
        std::max(padding.right(), inset.right()),
        std::max(padding.bottom(), inset.bottom()),
    };

    size = size.grownBy(spacing);
    if (size.width() < preferredSize.width()) {
        size.setWidth(preferredSize.width());
    }
    if (size.height() < preferredSize.height()) {
        size.setHeight(preferredSize.height());
    }

    return size.toSize();
}

QSizeF StyleGroupWrapper::size() const
{
    return QSizeF(width(), height());
}

qreal StyleGroupWrapper::width() const
{
    return safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::width);
}

qreal StyleGroupWrapper::height() const
{
    return safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::height);
}

QMarginsF StyleGroupWrapper::padding() const
{
    return safePropertyLookup(m_properties, QMarginsF{}, &StylePropertyGroup::layout, &LayoutPropertyGroup::padding, &SizePropertyGroup::toMargins);
}

qreal StyleGroupWrapper::averagePadding() const
{
    const auto p = padding();
    if (p.isNull()) {
        return 0;
    }
    return (p.left() + p.right() + p.top() + p.bottom()) / 4;
}

qreal StyleGroupWrapper::averageHPadding() const
{
    const auto p = padding();
    if (p.isNull()) {
        return 0;
    }
    return (p.left() + p.right()) / 2;
}

qreal StyleGroupWrapper::averageVPadding() const
{
    const auto p = padding();
    if (p.isNull()) {
        return 0;
    }
    return (p.top() + p.bottom()) / 2;
}

qreal StyleGroupWrapper::spacing() const
{
    return safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::layout, &LayoutPropertyGroup::spacing);
}

QMarginsF StyleGroupWrapper::borderSize() const
{
    return safePropertyLookup(m_properties, QMarginsF{}, &StylePropertyGroup::border, &BorderPropertyGroup::sizes);
}

qreal StyleGroupWrapper::averageBorderSize() const
{
    auto b = borderSize();
    if (b.isNull()) {
        return 0;
    }
    return (b.left() + b.right() + b.top() + b.bottom()) / 4;
}

QSizeF StyleGroupWrapper::iconSize() const
{
    auto width = safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::icon, &IconPropertyGroup::width);
    auto height = safePropertyLookup(m_properties, 0.0, &StylePropertyGroup::icon, &IconPropertyGroup::height);
    return QSizeF(width, height);
}

int StyleGroupWrapper::textFlags(Qt::LayoutDirection direction) const
{
    int textFlags = Qt::AlignVCenter;
    auto horizontalAlignment = toQtHorizontalAlignment(
        m_properties->safePropertyLookup(Alignment::Start, &StylePropertyGroup::layout, &LayoutPropertyGroup::alignment, &AlignmentPropertyGroup::horizontal));
    auto verticalAlignment = toQtVerticalAlignment(
        m_properties->safePropertyLookup(Alignment::Center, &StylePropertyGroup::layout, &LayoutPropertyGroup::alignment, &AlignmentPropertyGroup::vertical));

    horizontalAlignment = UnionStyle::visualAlignment(direction, horizontalAlignment);
    verticalAlignment = UnionStyle::visualAlignment(direction, verticalAlignment);

    const auto textAlign = horizontalAlignment | verticalAlignment;
    auto textWrap =
        toQtWrapMode(m_properties->safePropertyLookup(Union::Properties::TextWrapMode::NoWrap, &StylePropertyGroup::text, &TextPropertyGroup::wrapMode));
    textFlags |= textAlign;
    // Do not add wrap flags if we get DontClip
    // This could be done better
    if (textWrap != Qt::TextDontClip) {
        textFlags |= textWrap;
    }
    return textFlags;
}

// QRectF StyleGroupWrapper::visualRect(const QRectF &rect) const
// {
//     if (m_style && m_styleOption) {
//         return m_style->visualRect(m_styleOption->direction, m_styleOption->rect, rect);
//     }
//     return rect;
// }
