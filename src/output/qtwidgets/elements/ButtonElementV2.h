// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Akseli Lahtinen <akselmo@akselmo.dev>

#pragma once

#include "AbstractElementV2.h"

#include <QPushButton>

namespace Union
{
namespace Widgets
{

class ButtonElementV2 : public AbstractElementV2
{
    Q_OBJECT

public:
    ButtonElementV2(UnionStyle *style, QWidget *widget = nullptr);
    ~ButtonElementV2() override;

    Element::Ptr element() const override;

    void update(const QStyleOption *option) override;

    void draw(QPainter *painter, AbstractElementV2::DrawVariant what) const override;

private:
};

}
}
