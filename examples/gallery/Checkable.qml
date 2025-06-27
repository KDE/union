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

        Controls.RadioButton {
            text: "RadioButton with Text"
        }

        Controls.RadioButton {
            icon.name: "document-save"
            text: "RadioButton withh Text and Icon"
        }

        Controls.Switch {
            text: "Switch with Text"
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
