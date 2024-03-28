import QtQuick

import QtTest

import org.kde.union

// TestCase {
//
// }
Window {
    width: 800
    height: 600
    visible: true

    Column {
        anchors.centerIn: parent
        spacing: 10
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
}
