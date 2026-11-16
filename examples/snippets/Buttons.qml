// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2025 Arjen Hiemstra <ahiemstra@heimr.nl>

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import org.kde.union as Union

Union.ApplicationWindow {
    id: application

    title: "Union Buttons Overview"

    width: 1200
    height: 600
    visible: true

    property int display: Union.Button.TextBesideIcon

    header: Union.ToolBar {
        RowLayout {
            Union.Label {
                text: "Display:"
            }

            RowLayout {
                Layout.fillWidth: false
                spacing: 0

                Union.ButtonGroup {
                    id: displayGroup
                }

                component SegmentedButton: Union.ToolButton {
                    id: button

                    required property int value

                    checkable: true
                    checked: value == application.display

                    onToggled: if (checked) {
                        application.display = value
                    }

                    Union.ButtonGroup.group: displayGroup
                }

                SegmentedButton {
                    Union.Element.hints: Union.ElementHint { name: "segmented-start" }
                    value: Union.Button.IconOnly
                    text: "Icon Only"
                }

                SegmentedButton {
                    Union.Element.hints: Union.ElementHint { name: "segmented-middle" }
                    value: Union.Button.TextOnly
                    text: "Text Only"
                }

                SegmentedButton {
                    Union.Element.hints: Union.ElementHint { name: "segmented-middle" }
                    value: Union.Button.TextBesideIcon
                    text: "Text Beside Icon"
                }

                SegmentedButton {
                    Union.Element.hints: Union.ElementHint { name: "segmented-end" }
                    value: Union.Button.TextUnderIcon
                    text: "Text Under Icon"
                }
            }

            Union.Switch {
                id: enabledSwitch
                text: "Enabled"
                checked: true
            }

            Union.Switch {
                id: flatSwitch
                text: "Flat"
            }
        }
    }

    Union.ScrollView {
        anchors.fill: parent

        enabled: enabledSwitch.checked

        ListView {
            id: view

            model: [
                "",
                "Highlighted",
                "Changed",
                "Destructive",
                "Positive",
                "Negative",
            ]

            spacing: 20

            delegate: Union.Control {
                id: delegate

                required property var modelData

                width: ListView.view.width

                padding: 10

                contentItem: RowLayout {
                    spacing: 20

                    component HintButton: Union.Button {
                        required property string hint

                        Union.Element.states.highlighted: hint === "Highlighted"
                        Union.Element.hints: Union.ElementHint { name: hint !== "Highlighted" ? hint : "" }

                        display: application.display

                        flat: flatSwitch.checked

                        icon.name: "document-new"

                        activeFocusOnTab: false
                    }

                    Union.Label {
                        Layout.fillWidth: true

                        text: delegate.modelData || "Normal"
                    }

                    HintButton {
                        hint: delegate.modelData
                        text: "Interactive Button"
                        activeFocusOnTab: true
                        checkable: true
                    }

                    HintButton {
                        Union.Element.states {
                            hovered: false
                            visualFocus: false
                            pressed: false
                            checked: false
                        }
                        hint: delegate.modelData
                        text: "Normal Button"
                    }

                    HintButton {
                        Union.Element.states {
                            hovered: true
                            visualFocus: false
                            pressed: false
                            checked: false
                        }
                        hint: delegate.modelData
                        text: "Hovered Button"
                        visible: enabledSwitch.checked
                    }

                    HintButton {
                        Union.Element.states {
                            hovered: false
                            visualFocus: true
                            pressed: false
                            checked: false
                        }
                        hint: delegate.modelData
                        text: "Focused Button"
                        visible: enabledSwitch.checked
                    }

                    HintButton {
                        Union.Element.states {
                            hovered: false
                            visualFocus: false
                            pressed: true
                            checked: false
                        }
                        hint: delegate.modelData
                        text: "Pressed Button"
                        visible: enabledSwitch.checked
                    }

                    HintButton {
                        Union.Element.states {
                            hovered: false
                            visualFocus: false
                            pressed: false
                            checked: true
                        }
                        hint: delegate.modelData
                        text: "Checked Button"
                    }
                }
            }
        }
    }
}
