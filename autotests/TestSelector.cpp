// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2022 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <ranges>

#include <QtTest>

#include <Element.h>
#include <Selector.h>

#include "TestInputPlugin.h"

using namespace Union;
using namespace Qt::StringLiterals;

struct ElementProperties {
    QString type;
    QString id;
    Element::States states;
    QStringList hints;
    QVariantMap attributes;
};

ElementProperties jsonToElementProperties(const QJsonObject &json)
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
        Element::States states;
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

    return result;
}

ElementList structureFromJson(const QJsonArray &json)
{
    ElementList result;

    for (auto item : json) {
        auto properties = jsonToElementProperties(item.toObject());

        auto element = Element::create();
        element->setType(properties.type);
        element->setId(properties.id);
        element->setStates(properties.states);
        element->setHints(QStringList(properties.hints.begin(), properties.hints.end()));
        element->setAttributes(properties.attributes);

        result.append(element);
    }

    return result;
}

struct TestData {
    QString name;
    SelectorList selectors;
    ElementList structure;
    bool matchExpected = true;
};

QList<TestData> testDataFromJson()
{
    QList<TestData> result;

    auto data = QFINDTESTDATA("TestSelectorData");
    for (const auto &entry : std::filesystem::directory_iterator(data.toStdString())) {
        QFile file(entry.path());
        if (!file.open(QFile::ReadOnly)) {
            return result;
        }

        QJsonParseError error;
        auto json = QJsonDocument::fromJson(file.readAll(), &error);
        file.close();

        if (error.error != QJsonParseError::NoError) {
            qWarning() << "JSON file" << entry.path().filename().string() << "failed to parse:" << qPrintable(error.errorString());
            return result;
        }

        ElementList structure = structureFromJson(json[u"structure"].toArray());

        auto matching = json[u"matching_selectors"].toObject();

        const auto matching_keys = matching.keys();
        for (auto item : matching_keys) {
            SelectorList selectors = jsonToSelectorList(matching.value(item).toArray());
            QString name = QString::fromStdString(entry.path().stem()) + u"_matches_" + item;

            result.emplace_back(name, selectors, structure, true);
        }

        auto failing = json[u"failing_selectors"].toObject();
        const auto failing_keys = failing.keys();
        for (auto item : failing_keys) {
            SelectorList selectors = jsonToSelectorList(failing.value(item).toArray());
            QString name = QString::fromStdString(entry.path().stem()) + u"_no_match_" + item;

            result.emplace_back(name, selectors, structure, false);
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
        QVERIFY2(!emptySelector.matches(emptyElement.get()), "Empty selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::Type>(QString{});
        QVERIFY2(!emptySelector.matches(emptyElement.get()), "Empty Type selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::Id>(QString{});
        QVERIFY(!emptySelector.matches(emptyElement.get()));
        QVERIFY2(!emptySelector.matches(emptyElement.get()), "Empty ID selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::State>(Element::State::None);
        QVERIFY2(!emptySelector.matches(emptyElement.get()), "Empty State selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::Hint>(QString{});
        QVERIFY2(!emptySelector.matches(emptyElement.get()), "Empty Hint selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::AttributeExists>(QString{});
        QVERIFY2(!emptySelector.matches(emptyElement.get()), "Empty Attribute Exists selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::AttributeEquals>(std::make_pair(QString{}, QVariant{}));
        QVERIFY2(!emptySelector.matches(emptyElement.get()), "Empty Attribute Equals selector should not match an empty element");

        emptySelector = Selector::create<SelectorType::AttributeSubstringMatch>(std::make_pair(QString{}, QString{}));
        QVERIFY2(!emptySelector.matches(emptyElement.get()), "Empty Attribute Substring Match selector should not match an empty element");
    }

    void testSelectors_data()
    {
        QTest::addColumn<ElementList>("structure");
        QTest::addColumn<SelectorList>("selectors");
        QTest::addColumn<bool>("expected");

        const auto data = testDataFromJson();
        QVERIFY(!data.isEmpty());

        for (auto entry : data) {
            QTest::addRow("%s", entry.name.toUtf8().data()) << entry.structure << entry.selectors << entry.matchExpected;
        }
    }

    void testSelectors()
    {
        QFETCH(QList<Element::Ptr>, structure);
        QFETCH(SelectorList, selectors);
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

    void testDataStream()
    {
        const auto list = SelectorList{
            Selector::create<SelectorType::Type>(u"type"_s),
            Selector::create<SelectorType::Id>(u"id"_s),
            Selector::create<SelectorType::ChildCombinator>(),
            Selector::create<SelectorType::State>(Element::State::Highlighted),
            Selector::create<SelectorType::Hint>(u"hint"_s),
            Selector::create<SelectorType::DescendantCombinator>(),
            Selector::create<SelectorType::AttributeExists>(u"attribute"_s),
            Selector::create<SelectorType::AttributeEquals>(std::pair(u"attribute"_s, QVariant::fromValue(1.234))),
            Selector::create<SelectorType::AttributeSubstringMatch>(std::pair(u"other-attribute"_s, u"val"_s)),
            Selector::create<SelectorType::ChildCombinator>(),
            Selector::create<SelectorType::AnyElement>(),
        };

        QByteArray data;
        QDataStream writeStream(&data, QIODevice::WriteOnly);
        writeStream << list;

        QCOMPARE(writeStream.status(), QDataStream::Status::Ok);
        QVERIFY(data.size() > 0);

        QDataStream readStream(data);
        SelectorList readList;
        readStream >> readList;

        QCOMPARE(readStream.status(), QDataStream::Status::Ok);
        QVERIFY(readList.size() == list.size());

        for (int i = 0; i < readList.size(); ++i) {
            QCOMPARE(readList.at(i).toString(), list.at(i).toString());
        }
    }

    void benchmarkSelectors_data()
    {
        QTest::addColumn<QJsonArray>("structure");
        QTest::addColumn<SelectorList>("selectors");

        QTest::addRow("type") << QJsonArray{QJsonObject{{u"type"_s, u"type"_s}}} << SelectorList{Selector::create<SelectorType::Type>(u"type"_s)};
        QTest::addRow("id") << QJsonArray{QJsonObject{{u"id"_s, u"id"_s}}} << SelectorList{Selector::create<SelectorType::Id>(u"id"_s)};
        QTest::addRow("state") << QJsonArray{QJsonObject{{u"states"_s, u"hovered"_s}}}
                               << SelectorList{Selector::create<SelectorType::State>(Element::State::Hovered)};
        QTest::addRow("hint") << QJsonArray{QJsonObject{{u"hints"_s, QJsonArray{u"hint"_s}}}} << SelectorList{Selector::create<SelectorType::Hint>(u"hint"_s)};
        auto attribute = QJsonArray{QJsonObject{{u"attributes"_s, QJsonObject{{u"test"_s, u"value"_s}}}}};
        QTest::addRow("attribute_exists") << attribute << SelectorList{Selector::create<SelectorType::AttributeExists>(u"test"_s)};
        QTest::addRow("attribute_equals") << attribute
                                          << SelectorList{
                                                 Selector::create<SelectorType::AttributeEquals>(std::make_pair(u"test"_s, QVariant::fromValue(u"value"_s)))};
        QTest::addRow("attribute_substring") << attribute
                                             << SelectorList{Selector::create<SelectorType::AttributeSubstringMatch>(std::make_pair(u"test"_s, (u"lu"_s)))};

        auto structure = QJsonArray{
            QJsonObject{{u"type"_s, u"test"_s}},
            QJsonObject{{u"id"_s, u"id"_s}},
            QJsonObject{{u"states"_s, u"hovered"_s}},
        };
        QTest::addRow("child") << structure
                               << SelectorList{
                                      Selector::create<SelectorType::Id>(u"id"_s),
                                      Selector::create<SelectorType::ChildCombinator>(),
                                      Selector::create<SelectorType::State>(Element::State::Hovered),
                                  };
        QTest::addRow("descendant") << structure
                                    << SelectorList{
                                           Selector::create<SelectorType::Type>(u"test"_s),
                                           Selector::create<SelectorType::DescendantCombinator>(),
                                           Selector::create<SelectorType::State>(Element::State::Hovered),
                                       };
    }

    void benchmarkSelectors()
    {
        QFETCH(QJsonArray, structure);
        QFETCH(SelectorList, selectors);

        auto elements = structureFromJson(structure);

        QVERIFY(selectors.matches(elements));

        QBENCHMARK {
            selectors.matches(elements);
        }
    }
};

QTEST_MAIN(TestSelector)

#include "TestSelector.moc"
