// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{
class ToolBoxTabElement : public AbstractElement
{
    Q_OBJECT

public:
    ToolBoxTabElement(const UnionStyle *style, QWidget *widget);

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawVariant what = std::nullopt) const override;

    std::optional<QRectF> subElementRect(const QStyleOption *option, QStyle::SubElement element) const override;

private:
    void updateSubElementList() override;
    QString fallbackElementName(const QStyleOption *opt) const override;
    const QStyleOptionToolBox *m_toolBoxOption = nullptr;
};

}
}
