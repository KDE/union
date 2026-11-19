// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QStyleOption>

namespace Union
{
namespace Widgets
{

class AbstractElement;
class UnionStyle;

class TypeMapping
{
public:
    using ElementCreateFunction = std::function<AbstractElement *(const UnionStyle *, QWidget *)>;

    static AbstractElement *createElementForOption(const UnionStyle *style, int optionType);
    static AbstractElement *createElementForWidget(const UnionStyle *style, QWidget *widget);

    static void registerOptionToElement(int optionType, const ElementCreateFunction &function);
    static void registerWidgetToElement(const QByteArray &className, const ElementCreateFunction &function);

private:
    static QHash<int, ElementCreateFunction> s_optionToElementMap;
    static QHash<QByteArray, ElementCreateFunction> s_widgetToElementMap;
};

template<typename ElementType, typename... WidgetType>
struct WidgetToElementRegistration {
    WidgetToElementRegistration(const QByteArrayList &extraNames = {})
    {
        auto createFunction = [](const UnionStyle *style, QWidget *widget) {
            return new ElementType(style, widget);
        };

        (
            [&] {
                TypeMapping::registerWidgetToElement(WidgetType::staticMetaObject.className(), createFunction);
            }(),
            ... //
        );

        for (const auto &name : extraNames) {
            TypeMapping::registerWidgetToElement(name, createFunction);
        }
    }
};

template<typename ElementType, int... OptionType>
struct OptionToElementRegistration {
    OptionToElementRegistration()
    {
        auto createFunction = [](const UnionStyle *style, QWidget *widget) {
            return new ElementType(style, widget);
        };

        (
            [&] {
                TypeMapping::registerOptionToElement(OptionType, createFunction);
            }(),
            ... //
        );
    }
};

}
}
