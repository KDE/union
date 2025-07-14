// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <ranges>

#include <QtTest>

#include <Element.h>
#include <Selector.h>

using namespace Union;
using namespace Qt::StringLiterals;

Union::Element::State stateFromString(const QString &string)
{
    const auto metaEnum = QMetaEnum::fromType<Union::Element::States>();

    QByteArray value = string.toUtf8();

    auto count = metaEnum.keyCount();
    for (int i = 0; i < count; ++i) {
        if (qstrnicmp(metaEnum.key(i), value.data(), value.size()) == 0) {
            return Union::Element::State(metaEnum.value(i));
        }
    }

    return Union::Element::State::None;
}

struct ElementProperties {
    QString type;
    QString id;
    Union::Element::States states;
    QStringList hints;
    QVariantMap attributes;
    bool universal = false;
    bool child = false;
    bool descendant = false;
};

ElementProperties jsonToProperties(const QJsonObject &json)
{
    ElementProperties result;

    if (json.contains(u"type")) {
        result.type = json.value(u"type").toString();
    }

    if (json.contains(u"id")) {
        result.id = json.value(u"id").toString();
    }

    if (json.contains(u"states")) {
        QJsonArray statesArray;
        if (json.value(u"states").isArray()) {
            statesArray = json.value(u"states").toArray();
        } else {
            statesArray = {json.value(u"states")};
        }
        Union::Element::States states;
        for (auto state : statesArray) {
            states |= stateFromString(state.toString());
        }
        result.states = states;
    }

    if (json.contains(u"hints")) {
        const auto hintsArray = json.value(u"hints").toArray();
        QStringList hints;
        std::ranges::transform(hintsArray, std::back_inserter(hints), [](const auto &entry) {
            return entry.toString();
        });
        result.hints = hints;
    }

    if (json.contains(u"attributes")) {
        result.attributes = json.value(u"attributes").toObject().toVariantMap();
    }

    if (json.contains(u"universal")) {
        result.universal = json.value(u"universal").toBool();
    }

    if (json.contains(u"child")) {
        result.child = json.value(u"child").toBool();
    }

    if (json.contains(u"descendant")) {
        result.descendant = json.value(u"descendant").toBool();
    }

    return result;
}

SelectorList jsonToSelectorList(const QJsonArray &json)
{
    SelectorList result;
    for (auto entry : json) {
        auto properties = jsonToProperties(entry.toObject());

        if (properties.universal) {
            result.append(Union::Selector::create<Union::SelectorType::AnyElement>());
            continue;
        }

        if (properties.child) {
            result.append(Union::Selector::create<Union::SelectorType::ChildCombinator>());
            continue;
        }

        if (properties.descendant) {
            result.append(Union::Selector::create<Union::SelectorType::DescendantCombinator>());
            continue;
        }

        if (!properties.type.isEmpty()) {
            result.append(Union::Selector::create<Union::SelectorType::Type>(properties.type));
        }

        if (!properties.id.isEmpty()) {
            result.append(Union::Selector::create<Union::SelectorType::Id>(properties.id));
        }

        if (properties.states != 0) {
            QMetaEnum statesEnum = QMetaEnum::fromType<Union::Element::States>();
            const auto count = statesEnum.keyCount();
            for (int i = 0; i < count; ++i) {
                auto value = statesEnum.value(i);
                if (properties.states & value) {
                    result.append(Union::Selector::create<Union::SelectorType::State>(Union::Element::State(value)));
                }
            }
        }

        for (const auto &hint : properties.hints) {
            result.append(Union::Selector::create<Union::SelectorType::Hint>(hint));
        }
    }

    return result;
}

class TestSelector : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmptySelectors()
    {
        auto emptyElement = Element::create();

        Selector emptySelector = Selector::create();
        QVERIFY2(!emptySelector.matches(emptyElement), "Empty selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::Type>(QString{});
        QVERIFY2(!emptySelector.matches(emptyElement), "Empty Type selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::Id>(QString{});
        QVERIFY(!emptySelector.matches(emptyElement));
        QVERIFY2(!emptySelector.matches(emptyElement), "Empty ID selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::State>(Element::State::None);
        QVERIFY2(!emptySelector.matches(emptyElement), "Empty State selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::Hint>(QString{});
        QVERIFY2(!emptySelector.matches(emptyElement), "Empty Hint selector should not match an empty element");
    }

    // Note that this data function reads the JSON files under TestSelectorData
    // and converts them to QTest data.
    void testSelectors_data()
    {
        QTest::addColumn<QList<Union::Element::Ptr>>("structure");
        QTest::addColumn<Union::SelectorList>("selectors");
        QTest::addColumn<bool>("expected");

        auto data = QFINDTESTDATA("TestSelectorData");
        for (const auto &entry : std::filesystem::directory_iterator(data.toStdString())) {
            QFile file(entry.path());
            QVERIFY(file.open(QFile::ReadOnly));
            QJsonParseError error;
            auto json = QJsonDocument::fromJson(file.readAll(), &error);
            file.close();

            if (error.error != QJsonParseError::NoError) {
                auto message = std::format("JSON file {} failed to parse: {}", entry.path().filename().string(), error.errorString().toStdString());
                QFAIL(message.c_str());
            }

            auto structure_json = json[u"structure"].toArray();
            QList<Union::Element::Ptr> structure;
            for (auto item : structure_json) {
                auto properties = jsonToProperties(item.toObject());

                auto element = Union::Element::create();
                element->setType(properties.type);
                element->setId(properties.id);
                element->setStates(properties.states);
                element->setHints(properties.hints);
                element->setAttributes(properties.attributes);

                structure.append(element);
            }

            auto matching = json[u"matching_selectors"].toObject();

            const auto matching_keys = matching.keys();
            for (auto item : matching_keys) {
                Union::SelectorList selectors = jsonToSelectorList(matching.value(item).toArray());
                QString name = QString::fromStdString(entry.path().stem()) + u"_matches_" + item;
                QTest::addRow("%s", name.toUtf8().data()) << structure << selectors << true;
            }

            auto failing = json[u"failing_selectors"].toObject();
            const auto failing_keys = failing.keys();
            for (auto item : failing_keys) {
                Union::SelectorList selectors = jsonToSelectorList(failing.value(item).toArray());
                QString name = QString::fromStdString(entry.path().stem()) + u"_no_match_" + item;
                QTest::addRow("%s", name.toUtf8().data()) << structure << selectors << false;
            }
        }
    }

    void testSelectors()
    {
        QFETCH(QList<Union::Element::Ptr>, structure);
        QFETCH(Union::SelectorList, selectors);
        QFETCH(bool, expected);

        if (selectors.matches(structure) != expected) {
            if (expected) {
                auto message = std::format("Selector {} did not match structure {}", selectors, structure);
                QFAIL(message.c_str());
            } else {
                auto message = std::format("Selector {} matched structure {} but was not supposed to", selectors, structure);
                QFAIL(message.c_str());
            }
        }
    }
};

QTEST_MAIN(TestSelector)

#include "TestSelector.moc"
