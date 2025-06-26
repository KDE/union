import QtQuick
import QtQuick.Layouts

import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami

Kirigami.Page {
    title: "Checkable Controls QtQuick.Controls"

    ColumnLayout {
        anchors.fill: parent

        Controls.CheckBox {
            text: "Checkbox with Text"
        }

        Controls.CheckBox {
            icon.name: "document-save"
            text: "Checkbox with Text and Icon"
        }

        Item {
            Layout.fillHeight: true
        }
    }
}

