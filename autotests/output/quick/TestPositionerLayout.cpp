// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <QtTest>

#include "../../../src/output/qtquick/plugin/positioner/Layout.h"

using namespace std::string_literals;
using namespace Union::Quick;

template<>
struct std::formatter<QPointF> : public std::formatter<std::string, char> {
    template<class FormatContext>
    FormatContext::iterator format(QPointF point, FormatContext &context) const
    {
        std::ostringstream out;
        out << "(" << point.x() << ", " << point.y() << ")";
        return std::formatter<std::string, char>::format(std::move(out).str(), context);
    }
};

template<>
struct std::formatter<QSizeF> : public std::formatter<std::string, char> {
    template<class FormatContext>
    FormatContext::iterator format(QSizeF size, FormatContext &context) const
    {
        std::ostringstream out;
        out << "(" << size.width() << ", " << size.height() << ")";
        return std::formatter<std::string, char>::format(std::move(out).str(), context);
    }
};

template<typename T>
inline int toEnumIntValue(/* intentional copy */ std::string value)
{
    const auto metaEnum = QMetaEnum::fromType<T>();

    std::erase(value, '-');

    auto count = metaEnum.keyCount();
    for (int i = 0; i < count; ++i) {
        if (qstrnicmp(metaEnum.key(i), value.c_str(), value.size()) == 0) {
            return metaEnum.value(i);
        }
    }

    return -1;
}

// clang-format off
#define COMPARE_FORMAT(first, second, message, ...) \
    if (first != second) { \
        auto description = std::format(message, first, second, __VA_ARGS__); \
        QFAIL(description.data()); \
    }
// clang-format on

class TestPositionerLayout : public QObject
{
    Q_OBJECT

    struct ItemData {
        QPointF expectedPosition;
        QSizeF expectedSize;

        QSizeF implicitSize;
        QMarginsF margins;
        Union::Properties::Alignment verticalAlignment = Union::Properties::Alignment::Center;
        int order = 0;
    };

    struct BucketData {
        QPointF expectedPosition;
        QSizeF expectedSize;

        QList<ItemData> items;
    };

    struct ContainerData {
        QPointF expectedPosition;
        QSizeF expectedSize;

        std::optional<BucketData> start;
        std::optional<BucketData> center;
        std::optional<BucketData> end;
        std::optional<BucketData> fill;
    };

    struct LayoutData {
        QSizeF size;
        qreal spacing;
        QMarginsF padding;
        QMarginsF inset;

        std::optional<ContainerData> item;
        std::optional<ContainerData> background;
        std::optional<ContainerData> content;
    };

    struct TestData {
        std::string name;
        LayoutData layout;
        LayoutData expected;
    };

    std::pair<QPointF, QSizeF> positionSizeFromJson(const QJsonObject &json)
    {
        auto result = std::pair<QPointF, QSizeF>();

        if (json.contains(u"x") || json.contains(u"y")) {
            result.first = QPointF{json[u"x"].toDouble(), json[u"y"].toDouble()};
        }
        if (json.contains(u"width") || json.contains(u"height")) {
            result.second = QSizeF{json[u"width"].toDouble(), json[u"height"].toDouble()};
        }

        return result;
    }

    QMarginsF qMarginsFromJson(const QJsonValue &json)
    {
        if (json.isNull() || json.isUndefined() || !(json.isDouble() || json.isObject())) {
            return QMarginsF{};
        }

        if (json.isDouble()) {
            auto value = json.toDouble();
            return QMarginsF{value, value, value, value};
        }

        return QMarginsF(json[u"left"].toDouble(), json[u"top"].toDouble(), json[u"right"].toDouble(), json[u"bottom"].toDouble());
    }

    std::optional<ItemData> itemDataFromJson(const QJsonObject &json)
    {
        if (json.isEmpty()) {
            return std::nullopt;
        }

        ItemData data;
        auto [position, size] = positionSizeFromJson(json);
        data.expectedPosition = position;
        data.expectedSize = size;

        if (json.contains(u"implicitWidth") || json.contains(u"implicitHeight")) {
            data.implicitSize = QSizeF{json[u"implicitWidth"].toDouble(), json[u"implicitHeight"].toDouble()};
        }

        data.margins = qMarginsFromJson(json[u"margins"]);

        if (json.contains(u"verticalAlignment")) {
            data.verticalAlignment =
                Union::Properties::Alignment{toEnumIntValue<Union::Properties::Alignment>(json[u"verticalAlignment"].toString().toStdString())};
        }

        if (json.contains(u"order")) {
            data.order = json[u"order"].toInt();
        }

        return data;
    }

    std::optional<BucketData> bucketDataFromJson(const QJsonObject &json)
    {
        if (json.isEmpty()) {
            return std::nullopt;
        }

        BucketData data;
        auto [position, size] = positionSizeFromJson(json);
        data.expectedPosition = position;
        data.expectedSize = size;

        const auto items = json[u"items"].toArray();
        for (auto item : items) {
            auto itemData = itemDataFromJson(item.toObject());
            if (itemData) {
                data.items.append(itemData.value());
            }
        }

        return data;
    }

    std::optional<ContainerData> containerDataFromJson(const QJsonObject &json)
    {
        if (json.isEmpty()) {
            return std::nullopt;
        }

        ContainerData data;
        auto [position, size] = positionSizeFromJson(json);
        data.expectedPosition = position;
        data.expectedSize = size;

        data.start = bucketDataFromJson(json[u"start"].toObject());
        data.center = bucketDataFromJson(json[u"center"].toObject());
        data.end = bucketDataFromJson(json[u"end"].toObject());
        data.fill = bucketDataFromJson(json[u"fill"].toObject());

        return data;
    }

    std::optional<LayoutData> layoutDataFromJson(const QJsonObject &json)
    {
        if (json.isEmpty()) {
            return std::nullopt;
        }

        LayoutData data;
        data.size = QSizeF{json[u"width"].toDouble(), json[u"height"].toDouble()};
        data.spacing = json[u"spacing"].toDouble();
        data.padding = qMarginsFromJson(json[u"padding"]);
        data.inset = qMarginsFromJson(json[u"inset"]);

        data.item = containerDataFromJson(json[u"item"].toObject());
        data.background = containerDataFromJson(json[u"background"].toObject());
        data.content = containerDataFromJson(json[u"content"].toObject());

        return data;
    }

    QList<TestData> testDataFromJson()
    {
        QList<TestData> result;

        auto data = QFINDTESTDATA("TestPositionerLayoutData");
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
                QTest::Internal::maybeThrowOnFail();
                continue;
            }

            TestData data;
            data.name = entry.path().stem();

            auto layout = layoutDataFromJson(json.object()[u"layout"].toObject());
            if (!layout) {
                qWarning() << "Invalid layout in test file" << entry.path().filename().string();
                QTest::Internal::maybeThrowOnFail();
                continue;
            }
            data.layout = layout.value();

            auto expected = layoutDataFromJson(json.object()[u"expected"].toObject());
            if (!expected) {
                qWarning() << "Invalid expected data in test file" << entry.path().filename().string();
                QTest::Internal::maybeThrowOnFail();
                continue;
            }
            data.expected = expected.value();

            result.append(data);
        }

        return result;
    }

    Layout layoutFromData(const LayoutData &data)
    {
        Layout layout;
        layout.size = data.size;
        layout.spacing = data.spacing;
        layout.padding = data.padding;
        layout.inset = data.inset;

        const auto containers = {
            std::make_pair(&layout.itemContainer, data.item),
            std::make_pair(&layout.backgroundContainer, data.background),
            std::make_pair(&layout.contentContainer, data.content),
        };
        for (auto [container, containerData] : containers) {
            if (!containerData) {
                continue;
            }

            const auto buckets = {
                std::make_pair(&container->start, containerData->start),
                std::make_pair(&container->center, containerData->center),
                std::make_pair(&container->end, containerData->end),
                std::make_pair(&container->fill, containerData->fill),
            };
            for (auto [bucket, bucketData] : buckets) {
                if (!bucketData) {
                    continue;
                }

                for (auto itemData : std::as_const(bucketData->items)) {
                    LayoutItem item;
                    item.implicitSize = itemData.implicitSize;
                    item.margins = itemData.margins;
                    item.order = itemData.order;
                    item.verticalAlignment = itemData.verticalAlignment;

                    if (item.verticalAlignment == Union::Properties::Alignment::StackCenter) {
                        bucket->stackCenter = true;
                    } else if (item.verticalAlignment == Union::Properties::Alignment::StackFill) {
                        bucket->stackFill = true;
                    }

                    bucket->items.append(item);
                }
            }
        }

        return layout;
    }

private Q_SLOTS:
    void testLayout_data()
    {
        QTest::addColumn<LayoutData>("layoutData");
        QTest::addColumn<LayoutData>("expectedData");

        const auto data = testDataFromJson();
        QVERIFY(!data.isEmpty());

        for (auto entry : data) {
            QTest::addRow("%s", entry.name.data()) << entry.layout << entry.expected;
        }
    }

    void testLayout()
    {
        QFETCH(LayoutData, layoutData);
        QFETCH(LayoutData, expectedData);

        auto layout = layoutFromData(layoutData);
        layout.layout();

        const auto containers = {
            std::make_tuple("item"s, &layout.itemContainer, expectedData.item),
            std::make_tuple("background"s, &layout.backgroundContainer, expectedData.background),
            std::make_tuple("content"s, &layout.contentContainer, expectedData.content),
        };
        for (auto [containerName, container, expectedContainer] : containers) {
            if (!expectedContainer.has_value()) {
                auto description = std::format("Container {} expected to be empty, it is not", containerName);
                QVERIFY2(container->isEmpty(), description.data());
                continue;
            }

            COMPARE_FORMAT(container->position, expectedContainer->expectedPosition, "Container '{2}' position is {0}, expected {1}", containerName);
            COMPARE_FORMAT(container->size, expectedContainer->expectedSize, "Container '{2}' size is {0}, expected {1}", containerName);

            const auto buckets = {
                std::make_tuple("start"s, &container->start, expectedContainer->start),
                std::make_tuple("center"s, &container->center, expectedContainer->center),
                std::make_tuple("end"s, &container->end, expectedContainer->end),
                std::make_tuple("fill"s, &container->fill, expectedContainer->fill),
            };
            for (auto [bucketName, bucket, expectedBucket] : buckets) {
                if (!expectedBucket.has_value()) {
                    auto description = std::format("Bucket {} expected to be empty, it is not", bucketName);
                    QVERIFY2(bucket->isEmpty(), description.data());
                    continue;
                }

                COMPARE_FORMAT(bucket->position,
                               expectedBucket->expectedPosition,
                               "Bucket '{2}' of container '{3}' position is {0}, expected {1}",
                               bucketName,
                               containerName);
                COMPARE_FORMAT(bucket->size,
                               expectedBucket->expectedSize,
                               "Bucket '{2}' of container '{3}' size is {0}, expected {1}",
                               bucketName,
                               containerName);

                for (int i = 0; i < bucket->items.size(); ++i) {
                    auto item = bucket->items.at(i);
                    auto expectedItem = expectedBucket->items.at(i);

                    COMPARE_FORMAT(item.position,
                                   expectedItem.expectedPosition,
                                   "Item {2} of bucket '{3}' and container '{4}' position is {0}, expected {1}",
                                   i,
                                   bucketName,
                                   containerName);
                    COMPARE_FORMAT(item.size,
                                   expectedItem.expectedSize,
                                   "Item {2} of bucket '{3}' and container '{4}' size is {0}, expected {1}",
                                   i,
                                   bucketName,
                                   containerName);
                }
            }
        }
    }
};

QTEST_MAIN(TestPositionerLayout)

#include "TestPositionerLayout.moc"
