// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#pragma once

#include <QFile>
#include <QJsonArray>

#include <InputPlugin.h>
#include <Style.h>
#include <StyleLoader.h>
#include <StylePackage.h>

using namespace Union;
using namespace Qt::StringLiterals;

// Helper functions and classes for testing Style functionality.

// Convenience wrapper around QFINDTESTDATA that returns a path rather than a string.
#define FINDTESTPATH(relative) std::filesystem::path(QFINDTESTDATA(relative).toStdString())

Element::State stateFromString(const QString &string)
{
    const auto metaEnum = QMetaEnum::fromType<Element::States>();

    QByteArray value = string.toUtf8();

    auto count = metaEnum.keyCount();
    for (int i = 0; i < count; ++i) {
        if (qstrnicmp(metaEnum.key(i), value.data(), value.size()) == 0) {
            return Element::State(metaEnum.value(i));
        }
    }

    return Element::State::None;
}

Union::SelectorList jsonToSelectorList(const QJsonArray &json)
{
    Union::SelectorList result;
    for (const auto &entry : json) {
        if (entry[u"universal"].toBool()) {
            result.append(Selector::create<SelectorType::AnyElement>());
            continue;
        }

        if (entry[u"child"].toBool()) {
            result.append(Selector::create<SelectorType::ChildCombinator>());
            continue;
        }

        if (entry[u"descendant"].toBool()) {
            result.append(Selector::create<SelectorType::DescendantCombinator>());
            continue;
        }

        if (auto type = entry[u"type"].toString(); !type.isEmpty()) {
            result.append(Selector::create<SelectorType::Type>(type));
        }

        if (auto id = entry[u"id"].toString(); !id.isEmpty()) {
            result.append(Selector::create<SelectorType::Id>(id));
        }

        if (auto states = entry[u"states"]; !states.isUndefined()) {
            QJsonArray statesArray;
            if (states.isArray()) {
                statesArray = states.toArray();
            } else {
                statesArray = QJsonArray{states};
            }

            for (const auto &state : statesArray) {
                result.append(Selector::create<SelectorType::State>(stateFromString(state.toString())));
            }
        }

        for (const auto &hint : entry[u"hints"].toArray()) {
            result.append(Selector::create<SelectorType::Hint>(hint.toString()));
        }

        for (const auto &exists : entry[u"attributes_exists"].toArray()) {
            result.append(Selector::create<SelectorType::AttributeExists>(exists.toString()));
        }

        for (const auto &[key, value] : entry[u"attributes_equals"].toObject().asKeyValueRange()) {
            result.append(Selector::create<SelectorType::AttributeEquals>(std::make_pair(key.toString(), value.toVariant())));
        }

        for (const auto &[key, value] : entry[u"attributes_substring"].toObject().asKeyValueRange()) {
            result.append(Selector::create<SelectorType::AttributeSubstringMatch>(std::make_pair(key.toString(), value.toString())));
        }
    }

    return result;
}

Union::Color variantToColor(const QVariant &variant)
{
    auto qcolor = variant.value<QColor>();
    return Union::Color::rgba(qcolor.red(), qcolor.green(), qcolor.blue(), qcolor.alpha());
}

static const QStringList KnownProperties{
    u"background-color"_s,
};

void setRuleProperty(const std::shared_ptr<Union::StyleRule> &rule, const QString &key, const QVariant &value)
{
    if (!KnownProperties.contains(key)) {
        return;
    }

    if (!rule->properties()) {
        rule->setProperties(std::make_unique<Union::Properties::StylePropertyGroup>());
    }
    auto properties = rule->properties();

    if (key.startsWith(u"background")) {
        if (!properties->background()) {
            properties->setBackground(std::make_unique<Union::Properties::BackgroundPropertyGroup>());
        }
        auto background = properties->background();

        if (key == u"background-color") {
            background->setColor(variantToColor(value));
        }
    }
}

struct TestStyleLoader : public Union::StyleLoader {
    bool load(std::shared_ptr<Union::Style> style) override
    {
        auto stylePath = style->path() / "contents" / "test" / "style.json";

        QFile jsonFile{stylePath};
        if (!jsonFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed reading test style data from" << stylePath.string() << jsonFile.errorString();
            return false;
        }

        const auto json = QJsonDocument::fromJson(jsonFile.readAll())[u"rules"].toArray();
        if (json.isEmpty()) {
            qWarning() << stylePath.string() << "does not contain valid JSON data";
            return false;
        }

        for (const auto &jsonRule : json) {
            auto rule = Union::StyleRule::create();

            for (const auto &[key, value] : jsonRule.toObject().asKeyValueRange()) {
                if (key == u"selectors") {
                    rule->setSelectors(jsonToSelectorList(value.toArray()));
                } else {
                    setRuleProperty(rule, key.toString(), value.toVariant());
                }
            }

            if (!rule->isEmpty()) {
                style->insert(rule);
            }
        }

        return true;
    }
};

struct TestInputPlugin : public Union::InputPlugin {
    Union::StylePackage::Error validatePackage([[maybe_unused]] const Union::StylePackage &package) override
    {
        if (!std::filesystem::exists(package.path() / "contents" / "test")) {
            return Union::StylePackage::Error::MissingFiles;
        }

        if (!std::filesystem::exists(package.path() / "contents" / "test" / "style.json")) {
            return Union::StylePackage::Error::MissingFiles;
        }

        return Union::StylePackage::Error::None;
    }

    bool createPackage([[maybe_unused]] const Union::StylePackage &package, [[maybe_unused]] std::error_code &errorCode) override
    {
        return true;
    }

    std::shared_ptr<Union::Style> createStyle(const Union::StylePackage &package) const override
    {
        return Union::Style::create(package.path(), std::make_unique<TestStyleLoader>());
    }
};
