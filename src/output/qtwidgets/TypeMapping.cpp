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
        if (factoryFunction) {
            qDebug() << "found factory function for" << widget->metaObject()->className() << "from" << metaObject->className();
        }
        metaObject = metaObject->superClass();
    }

    if (factoryFunction) {
        auto element = factoryFunction(style, widget);
        qDebug() << "Created" << element << "for" << widget;
        return element;
    } else {
        return nullptr;
    }
}

void TypeMapping::registerOptionToElement(int optionType, const ElementCreateFunction &function)
{
    qDebug() << "Type registration for" << optionType;
    s_optionToElementMap.insert(optionType, function);
}

void Union::Widgets::TypeMapping::registerWidgetToElement(const QByteArray &className, const ElementCreateFunction &function)
{
    qDebug() << "Type registration for" << className;
    s_widgetToElementMap.insert(className, function);
}
