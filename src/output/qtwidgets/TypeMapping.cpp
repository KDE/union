// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include "TypeMapping.h"

using namespace Union::Widgets;

QHash<int, TypeMapping::ElementCreateFunction> TypeMapping::s_optionToElementMap;
QHash<QByteArray, TypeMapping::ElementCreateFunction> TypeMapping::s_widgetToElementMap;

AbstractElement *TypeMapping::createElementForOption(const UnionStyle *style, int optionType)
{
    auto factoryFunction = s_optionToElementMap.value(optionType);
    if (factoryFunction) {
        return factoryFunction(style, nullptr);
    } else {
        return nullptr;
    }
}

AbstractElement *TypeMapping::createElementForWidget(const UnionStyle *style, QWidget *widget)
{
    ElementCreateFunction factoryFunction;
    const QMetaObject *metaObject = widget->metaObject();
    while (metaObject && !factoryFunction) {
        factoryFunction = s_widgetToElementMap.value(metaObject->className());
        metaObject = metaObject->superClass();
    }

    if (factoryFunction) {
        return factoryFunction(style, widget);
    } else {
        qCDebug(UNION_QTWIDGETS) << "No element type found for widget type" << widget->metaObject()->className();
        return nullptr;
    }
}

void TypeMapping::registerOptionToElement(int optionType, const ElementCreateFunction &function)
{
    s_optionToElementMap.insert(optionType, function);
}

void Union::Widgets::TypeMapping::registerWidgetToElement(const QByteArray &className, const ElementCreateFunction &function)
{
    s_widgetToElementMap.insert(className, function);
}
