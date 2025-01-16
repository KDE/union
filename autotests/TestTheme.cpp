// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <QtTest>

#include <Theme.h>
#include <ThemeLoader.h>

using namespace Union;
using namespace Qt::StringLiterals;

struct TestLoader : public ThemeLoader {
    bool load(std::shared_ptr<Theme> theme) override
    {
        auto testRule = StyleRule::create();
        testRule->setSelectors({Selector::create<SelectorType::Id>(u"test"_s)});
        theme->insert(testRule);

        return true;
    }
};

class TestTheme : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testLoad()
    {
        auto theme = Theme::create(u"test"_s, u"test"_s, std::make_unique<TestLoader>());

        QVERIFY(theme->load());

        const auto rules = theme->rules();

        QCOMPARE(rules.size(), 1);
    }

    void testMatches()
    {
        auto theme = Theme::create(u"test"_s, u"test"_s, std::make_unique<TestLoader>());
        QVERIFY(theme->load());

        QList<Element::Ptr> elements;
        auto element = Element::create();
        element->setId(u"test"_s);
        elements.append(element);

        auto result = theme->matches(elements);
        QCOMPARE(result.size(), 1);

        elements.clear();
        element = Element::create();
        element->setType(u"test"_s);
        elements.append(element);

        result = theme->matches(elements);
        QCOMPARE(result.size(), 0);
    }
};

QTEST_MAIN(TestTheme)

#include "TestTheme.moc"
