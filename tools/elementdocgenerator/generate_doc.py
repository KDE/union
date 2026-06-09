#!/usr/bin/env python

# SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
# SPDX-FileCopyrightText: 2026 Arjen Hiemstra <ahiemstra@heimr.nl>

import dataclasses
from pathlib import Path
from typing import Any

import ruamel.yaml as yaml
import jinja2


base_directory = Path(__file__).parent
root_directory = base_directory.parent.parent
doc_directory = root_directory / "doc"


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


@dataclasses.dataclass
class Hint:
    name: str
    description: str


@dataclasses.dataclass
class Attribute:
    name: str
    description: str = ""
    values: dict[str, str] = dataclasses.field(default_factory=dict)


@dataclasses.dataclass
class Element:
    name: str = ""
    group: str = ""
    description: str = ""

    type: str = ""
    states: list[str] = dataclasses.field(default_factory=list)
    hints: list[Hint] = dataclasses.field(default_factory=list)
    attributes: list[Attribute] = dataclasses.field(default_factory=list)
    subelements: list["Element"] = dataclasses.field(default_factory=list)

    def has_details(self):
        return self.type or self.states or self.hints or self.attributes


def process_node(node):
    if not isinstance(node, yaml.MappingNode):
        raise RuntimeError(f"Node {node} is not a mapping node")

    element = Element()

    for key_node, value_node in node.value:
        if key_node.value == "name":
            element.name = value_node.value

        if key_node.value == "group":
            element.group = value_node.value

        if key_node.value == "description":
            element.description = value_node.value

        if key_node.value == "type":
            element.type = value_node.value

        if key_node.value == "states":
            for node in value_node.value:
                element.states.append(node.value)

        if key_node.value == "hints":
            for hint_name, hint_description in value_node.value:
                element.hints.append(Hint(hint_name.value, hint_description.value))

        if key_node.value == "attributes":
            for attribute_name, attribute_description in value_node.value:
                attribute = Attribute(attribute_name.value)
                attribute.description = mapping_value(attribute_description, "description", "")

                values = mapping_value(attribute_description, "values", [])
                for key, value in values:
                    attribute.values[key.value] = value.value

                element.attributes.append(attribute)

        if key_node.value == "subelements":
            for node in value_node.value:
                element.subelements.append(process_node(node))

    return element


def render_template(template_name: str, output_path: Path, env: jinja2.Environment, data: dict):
    render_data = data.copy()
    render_data["extra_code"] = data.get("extra_code", {}).get(template_name, "")
    render_data["system_includes"] = data.get("system_includes", {}).get(template_name, [])
    render_data["local_includes"] = data.get("local_includes", {}).get(template_name, [])
    render_data["api_documentation"] = data.get("api_documentation", "")
    render_data["css_documentation"] = data.get("css_documentation", "")

    with open(output_path, "w") as f:
        template = jinja_env.get_template(template_name, None)
        f.write(template.render(render_data))


if __name__ == "__main__":
    structure = None

    parser = yaml.YAML()

    types = []

    for path in sorted(base_directory.glob("*.yml")):
        with open(path) as f:
            structure = parser.compose(f)

            for node in structure.value:
                types.append(process_node(node))

    jinja_env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(base_directory),
        autoescape=False,
        trim_blocks=True,
        lstrip_blocks=True,
    )

    output_path = doc_directory / "elements.qdoc"
    if output_path.exists():
        output_path.unlink()

    render_template("elements.qdoc.j2", output_path, jinja_env, {"types": types})
