import QtQuick
import QtQuick.Layouts

import QtTest

import org.kde.union

// TestCase {
//
// }
ApplicationWindow {
    width: 800
    height: 600
    visible: true

    header: ToolBar {
        Element.elementId: "window-header"

        RowLayout {
            ToolButton {
                text: "Header ToolButton 1"
            }
            ToolButton {
                text: "Header ToolButton 2"
            }
        }
    }

    Column {
        Element.elementId: "window-contents"

        anchors.centerIn: parent
        spacing: Element.spacing

        Button {
            width: 200
            height: 50
            text: "Test"
        }
        Button {
            width: 200
            height: 50
            text: "Test 2"
        }
    }

    footer: ToolBar {
        Element.elementId: "window-footer"

        TextField {
            anchors.fill: parent
        }
    }
}
