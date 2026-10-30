// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T

import org.kde.union.impl as Union

QtObject {
    id: root

    // Either SpinBox or DoubleSpinBox, but we need to use var because the two
    // types have no relation to each other.
    required property var control

    // Determine if we are "space constrained", that is, the text that gets
    // displayed wouldn't fit if both indicators are displayed next to each
    // other. This can be used by styles to switch to a more compact layout
    // in that case.
    readonly property bool constrained: {
        let constrained = Infinity

        // Use preferredWidth if set, or maximumWidth if preferred is not set.
        if (root.control.Layout.preferredWidth > 0) {
            constrained = root.control.Layout.preferredWidth
        } else if (root.control.Layout.maximumWidth < Infinity) {
            constrained = root.control.Layout.maximumWidth
        }

        return (Math.max(fromRect.width, toRect.width) + unconstrainedIncreaseIndicator.width + unconstrainedDecreaseIndicator.width + root.control.spacing * 2) > constrained
    }

    function valueRect(value)
    {
        let text = ""
        if (root.control.hasOwnProperty("decimals")) {
            text = root.control.textFromValue(value, root.control.decimals, root.control.locale)
        } else {
            text = root.control.textFromValue(value, root.control.locale)
        }
        return metrics.boundingRect(text)
    }

    readonly property rect fromRect: root.valueRect(root.control.from)
    readonly property rect toRect: root.valueRect(root.control.to)

    readonly property FontMetrics metrics: FontMetrics {
        font: root.control.font
    }

    // Helper to retrieve the size of indicators when not constrained.
    // Since the indicator size may changed based on the constrained hint,
    // we need a baseline to use in comparisons.
    readonly property Item unconstrained: Item {
        Union.Element.type: "SpinBox"

        Item {
            id: unconstrainedIncreaseIndicator
            Union.Element.subElement: "Increase"
            width: Union.Style.properties.layout.width
        }

        Item {
            id: unconstrainedDecreaseIndicator
            Union.Element.subElement: "Decrease"
            width: Union.Style.properties.layout.width
        }
    }
}
