// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElement.h"

namespace Union
{
namespace Widgets
{
class StatusBarElement;

class StatusBarElement : public AbstractElement
{
    Q_OBJECT

public:
    StatusBarElement(const UnionStyle *style, const QWidget *widget = nullptr);
    ~StatusBarElement() override;

    void update(const QStyleOption *option) override;
    void layout() override;
    void draw(QPainter *painter, DrawEnums enums) const override;

private:
    void drawItem(QPainter *painter) const;
    QString fallbackElementName(const QStyleOption *opt) const override;
    void updateSubElementList() override;
    Union::ElementList m_contentElementList;
    Union::StylePropertyGroupPtr m_contentProperties = nullptr;
};
}
}
