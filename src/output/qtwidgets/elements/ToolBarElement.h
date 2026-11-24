// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{

class ToolBarElement;

class ToolBarElement : public AbstractElement
{
    Q_OBJECT

public:
    ToolBarElement(const UnionStyle *style, const QWidget *widget);

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;
    qreal pixelMetric(QStyle::PixelMetric pixelMetric) const override;

private:
    Union::IdentifierList elementHints() const override;
    void updateSubElementList() override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    void drawHandle(QPainter *painter) const;
    void drawSeparator(QPainter *painter) const;
    QRectF handleSeparatorRect(qreal extent) const;
    const QStyleOptionToolBar *m_toolBarOption = nullptr;

    Union::ElementList m_handleElementList;
    Union::StylePropertyGroupPtr m_handleProperties;
    Union::ElementList m_separatorElementList;
    Union::StylePropertyGroupPtr m_separatorProperties;
    Union::ElementList m_extensionElementList;
    Union::StylePropertyGroupPtr m_extensionProperties;

    qreal extent(Union::StylePropertyGroupPtr properties) const;
};
}
}
