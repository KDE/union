// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

#include <QtTest>

#include <QQmlComponent>
#include <QQmlEngine>
#include <QStandardItemModel>

#include <Style.h>
#include <StyleLoader.h>
#include <StyleRegistry.h>

using namespace Qt::StringLiterals;

class TestStyleLoader : public Union::StyleLoader
{
public:
    bool load([[maybe_unused]] std::shared_ptr<Union::Style> theme) override
    {
        return true;
    }
};

class TestControls : public QObject
{
    Q_OBJECT

    QQmlEngine m_engine;

private Q_SLOTS:
    void initTestCase()
    {
        qputenv("UNION_DISABLE_INPUT_PLUGINS", "1");
        qputenv("UNION_STYLE_PLUGIN", "test");
        qputenv("UNION_STYLE_NAME", "test");

        Union::StyleRegistry::instance()->load();

        auto testStyle = Union::Style::create(u"test"_s, u"test"_s, std::make_unique<TestStyleLoader>());
        Union::StyleRegistry::instance()->addStyle(testStyle);
    }

    void testControls_data()
    {
        QTest::addColumn<QString>("control");
        QTest::addColumn<QVariantMap>("initialProperties");

        QTest::addRow("AbstractButton") << "AbstractButton" << QVariantMap{};
        QTest::addRow("ActionGroup") << "ActionGroup" << QVariantMap{};
        QTest::addRow("Action") << "Action" << QVariantMap{};
        QTest::addRow("ApplicationWindow") << "ApplicationWindow" << QVariantMap{};
        QTest::addRow("BusyIndicator") << "BusyIndicator" << QVariantMap{};
        QTest::addRow("Button") << "Button" << QVariantMap{};
        QTest::addRow("CheckBox") << "CheckBox" << QVariantMap{};
        QTest::addRow("CheckDelegate") << "CheckDelegate" << QVariantMap{};
        QTest::addRow("ComboBox") << "ComboBox" << QVariantMap{};
        QTest::addRow("Container") << "Container" << QVariantMap{};
        QTest::addRow("Control") << "Control" << QVariantMap{};
        QTest::addRow("DialogButtonBox") << "DialogButtonBox" << QVariantMap{};
        QTest::addRow("Dialog") << "Dialog" << QVariantMap{};
        QTest::addRow("Drawer") << "Drawer" << QVariantMap{};
        QTest::addRow("Frame") << "Frame" << QVariantMap{};
        QTest::addRow("GroupBox") << "GroupBox" << QVariantMap{};
        QTest::addRow("HorizontalHeaderView") << "HorizontalHeaderView" << QVariantMap{};
        QTest::addRow("ItemDelegate") << "ItemDelegate" << QVariantMap{};
        QTest::addRow("Label") << "Label" << QVariantMap{};
        QTest::addRow("MenuBarItem") << "MenuBarItem" << QVariantMap{};
        QTest::addRow("MenuBar") << "MenuBar" << QVariantMap{};
        QTest::addRow("MenuItem") << "MenuItem" << QVariantMap{};
        QTest::addRow("Menu") << "Menu" << QVariantMap{};
        QTest::addRow("MenuSeparator") << "MenuSeparator" << QVariantMap{};
        QTest::addRow("PageIndicator") << "PageIndicator" << QVariantMap{};
        QTest::addRow("Page") << "Page" << QVariantMap{};
        QTest::addRow("Pane") << "Pane" << QVariantMap{};
        QTest::addRow("Popup") << "Popup" << QVariantMap{};
        QTest::addRow("ProgressBar") << "ProgressBar" << QVariantMap{};
        QTest::addRow("RadioButton") << "RadioButton" << QVariantMap{};
        QTest::addRow("RadioDelegate") << "RadioDelegate" << QVariantMap{};
        QTest::addRow("RangeSlider") << "RangeSlider" << QVariantMap{};
        QTest::addRow("RoundButton") << "RoundButton" << QVariantMap{};
        QTest::addRow("ScrollBar") << "ScrollBar" << QVariantMap{};
        QTest::addRow("ScrollIndicator") << "ScrollIndicator" << QVariantMap{};
        QTest::addRow("ScrollView") << "ScrollView" << QVariantMap{};
        QTest::addRow("SelectionRectangle") << "SelectionRectangle" << QVariantMap{};
        QTest::addRow("Slider") << "Slider" << QVariantMap{};
        QTest::addRow("SpinBox") << "SpinBox" << QVariantMap{};
        QTest::addRow("SplitView") << "SplitView" << QVariantMap{};
        QTest::addRow("StackView") << "StackView" << QVariantMap{};
        QTest::addRow("SwipeDelegate") << "SwipeDelegate" << QVariantMap{};
        QTest::addRow("SwipeView") << "SwipeView" << QVariantMap{};
        QTest::addRow("SwitchDelegate") << "SwitchDelegate" << QVariantMap{};
        QTest::addRow("Switch") << "Switch" << QVariantMap{};
        QTest::addRow("TabBar") << "TabBar" << QVariantMap{};
        QTest::addRow("TabButton") << "TabButton" << QVariantMap{};
        QTest::addRow("TextArea") << "TextArea" << QVariantMap{};
        QTest::addRow("TextField") << "TextField" << QVariantMap{};
        QTest::addRow("ToolBar") << "ToolBar" << QVariantMap{};
        QTest::addRow("ToolButton") << "ToolButton" << QVariantMap{};
        QTest::addRow("ToolSeparator") << "ToolSeparator" << QVariantMap{};
        QTest::addRow("ToolTip") << "ToolTip" << QVariantMap{};
        QTest::addRow("VerticalHeaderView") << "VerticalHeaderView" << QVariantMap{};

        // These have required properties and thus require initial properties
        // auto testModel = new QStandardItemModel{};
        QTest::addRow("TableViewDelegate") << "TableViewDelegate"
                                           << QVariantMap{
                                                  {u"row"_s, 0},
                                                  {u"column"_s, 0},
                                                  {u"selected"_s, false},
                                                  {u"current"_s, false},
                                                  {u"editing"_s, false},
                                                  {u"model"_s, QVariantMap{}},
                                                  {u"tableView"_s, QVariant::fromValue(nullptr)},
                                              };
        QTest::addRow("TreeViewDelegate") << "TreeViewDelegate"
                                          << QVariantMap{
                                                 {u"row"_s, 0},
                                                 {u"depth"_s, 0},
                                                 {u"selected"_s, false},
                                                 {u"current"_s, false},
                                                 {u"editing"_s, false},
                                                 {u"expanded"_s, false},
                                                 {u"hasChildren"_s, false},
                                                 {u"isTreeNode"_s, false},
                                                 {u"model"_s, QVariantMap{}},
                                                 {u"treeView"_s, QVariant::fromValue(nullptr)},
                                             };
    }

    void testControls()
    {
        QFETCH(QString, control);
        QFETCH(QVariantMap, initialProperties);

        QQmlComponent component(&m_engine);
        component.setData(QString(u"import org.kde.union\n" + control + u" { }").toUtf8(), QUrl{});

        if (component.isError()) {
            for (auto error : component.errors()) {
                qCritical() << error.toString();
            }
            QFAIL("Component failed to load");
        }

        auto instance = component.createWithInitialProperties(initialProperties, m_engine.rootContext());
        if (!instance) {
            for (auto error : component.errors()) {
                qCritical() << error.toString();
            }
            QFAIL("Could not create an instance of component");
        }
    }
};

QTEST_MAIN(TestControls)

#include "TestControls.moc"
