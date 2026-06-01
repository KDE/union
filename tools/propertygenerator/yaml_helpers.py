# SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
# SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

from typing import Any

import ruamel.yaml as yaml


class AliasNode(yaml.Node):
    id = "anchor"


class PreserveAliasesComposer(yaml.composer.Composer):
    def compose_node(self, parent, index):
        if self.parser.check_event(yaml.AliasEvent):
            event = self.parser.get_event()
            return AliasNode("", event.anchor, event.start_mark, event.end_mark)

        event = self.parser.peek_event()
        result = super().compose_node(parent, index)

        if event.anchor is not None:
            result.anchor = event.anchor
        else:
            result.anchor = None

        return result


def mapping_value_node(mapping_node: yaml.MappingNode, key: str) -> yaml.Node | None:
    for key_node, value_node in mapping_node.value:
        if key_node.value == key:
            return value_node

    return None


def mapping_value(mapping_node: yaml.MappingNode, key: str, default_value: Any = None) -> Any:
    node = mapping_value_node(mapping_node, key)
    if node is not None:
        return node.value
    else:
        return default_value
