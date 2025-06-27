#!/usr/bin/env python

# SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import dataclasses
from pathlib import Path
import shutil
from typing import Optional, Type
import re

import ruamel.yaml as yaml
import jinja2

base_directory = Path(__file__).parent
root_directory = base_directory.parent.parent
src_directory = root_directory / "src" / "properties"
tests_directory = root_directory / "autotests" / "properties"
css_input_directory = root_directory / "src" / "input" / "css" / "defaults"
quick_output_directory = root_directory / "src" / "output" / "qtquick" / "plugin" / "properties"

include_patterns = [
    {"pattern": "qreal", "use_include": None},
    {"pattern": "int", "use_include": None},
    {"pattern": "Qt::", "use_include": "QtGlobal", "system_include": True},
    {"pattern": "Q", "system_include": True},
    {"pattern": "Union::Properties::", "use_include": "../PropertiesTypes.h"},
    {"pattern": "", },
]


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


@dataclasses.dataclass
class PropertyDescription:
    name: str
    type: str = ""
    type_object: Optional[Type] = None
    group: Optional["GroupDescription"] = None


@dataclasses.dataclass
class GroupDescription:
    name: str
    type_name: str
    parent_group: Optional["GroupDescription"] = None
    properties: list[PropertyDescription] = dataclasses.field(default_factory=list)
    system_includes: dict[str, set[str]] = dataclasses.field(default_factory=dict)
    local_includes: dict[str, set[str]] = dataclasses.field(default_factory=dict)
    extra_code: dict[str, str] = dataclasses.field(default_factory=dict)
    documentation: str = ""

    def __lt__(self, other):
        return self.type_name < other.type_name


def ucfirst(value):
    return f"{value[0].upper()}{value[1:]}"


def qualified_name(type_name):
    return f"{ucfirst(type_name)}Property"


def css_name(name):
    parts = re.split(r"([A-Z][a-z]+)", name)
    return "-".join(part.lower() for part in parts if part)


def process_node(node, name, parent_group, memo):
    if not isinstance(node, yaml.MappingNode):
        raise RuntimeError(f"Node {node} is not a mapping node!")

    type_name = qualified_name(name)

    description = GroupDescription(name, type_name, parent_group)
    memo[name] = description

    if (node.comment and node.comment[-1]):
        comments = node.comment[-1]

        documentation = ""
        for line in comments:
            text = line.value.strip().lstrip("# ")

            if text.startswith("SPDX"):
                continue

            documentation += text
            documentation += "\n"

        description.documentation = documentation

    for key_node, value_node in node.value:
        if key_node.value == "_extra_code":
            for template_name, extra_code in value_node.value:
                if isinstance(extra_code, yaml.MappingNode):
                    extra_code_data = {}
                    for identifier, code in extra_code.value:
                        extra_code_data[identifier.value] = code.value
                    description.extra_code[template_name.value] = extra_code_data
                else:
                    description.extra_code[template_name.value] = extra_code.value
            continue

        if key_node.value == "_extra_system_includes":
            for template_name, includes in value_node.value:
                for include_name in includes.value:
                    if template_name.value not in description.system_includes:
                        description.system_includes[template_name.value] = set()
                    description.system_includes[template_name.value].add(include_name.value)
            continue

        prop = PropertyDescription(name = key_node.value, group = description)

        match type(value_node).__name__:
            case "MappingNode":
                name = value_node.anchor if value_node.anchor is not None else key_node.value
                memo = memo | process_node(value_node, name, description, memo)

                prop.type = qualified_name(name)
                prop.type_object = memo[name]
                prop.is_group = True

            case "AliasNode":
                group = memo[value_node.value]

                prop.type = group.type_name
                prop.type_object = group
                prop.is_group = True

            case "ScalarNode":
                prop.type = value_node.value

        for entry in include_patterns:
            if not prop.type.startswith(entry["pattern"]):
                continue

            system = entry.get("system_include", False)
            include = entry.get("use_include", prop.type if system else prop.type + ".h")

            if include is None:
                break

            if system:
                if "property.h.j2" not in description.system_includes:
                    description.system_includes["property.h.j2"] = set()
                description.system_includes["property.h.j2"].add(include)
            else:
                if "property.h.j2" not in description.local_includes:
                    description.local_includes["property.h.j2"] = set()
                description.local_includes["property.h.j2"].add(include)

            break

        description.properties.append(prop)

    return memo


@jinja2.pass_context
def render_template_filter(context, value):
    return context.environment.from_string(value).render(context)


def render_template(template_name: str, output_path: Path, env: jinja2.Environment, data: dict):
    render_data = data.copy()
    render_data["extra_code"] = data.get("extra_code", {}).get(template_name, "")
    render_data["system_includes"] = data.get("system_includes", {}).get(template_name, [])
    render_data["local_includes"] = data.get("local_includes", {}).get(template_name, [])
    render_data["documentation"] = data.get("documentation", "")

    with open(output_path, "w") as f:
        template = jinja_env.get_template(template_name, None)
        f.write(template.render(render_data))


if __name__ == "__main__":
    structure = None

    parser = yaml.YAML()
    parser.Composer = PreserveAliasesComposer

    with open(base_directory / "properties.yml") as f:
        structure = parser.compose(f)

    types = process_node(structure, "style", None, {})

    jinja_env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(base_directory),
        autoescape=False,
        trim_blocks=True,
        lstrip_blocks=True,
    )
    jinja_env.filters["ucfirst"] = ucfirst
    jinja_env.filters["render"] = render_template_filter
    jinja_env.filters["css_name"] = css_name

    shutil.rmtree(src_directory, ignore_errors = True)
    shutil.rmtree(tests_directory, ignore_errors = True)
    shutil.rmtree(quick_output_directory, ignore_errors = True)

    css_generated_path = css_input_directory / "generated-properties.css"
    if css_generated_path.exists():
        css_generated_path.unlink()

    src_directory.mkdir(exist_ok = True)
    tests_directory.mkdir(exist_ok = True)
    css_input_directory.mkdir(exist_ok = True)
    quick_output_directory.mkdir(exist_ok = True)

    for name, type_definition in types.items():
        data = {field.name: getattr(type_definition, field.name) for field in dataclasses.fields(type_definition)}

        type_name = type_definition.type_name

        render_template("property.h.j2", (src_directory / type_name).with_suffix(".h"), jinja_env, data)
        render_template("property.cpp.j2", (src_directory / type_name).with_suffix(".cpp"), jinja_env, data)

        render_template("autotest.cpp.j2", (tests_directory / ("Test" + type_name)).with_suffix(".cpp"), jinja_env, data)

        render_template("qml_group.h.j2", (quick_output_directory / (type_name + "Group")).with_suffix(".h"), jinja_env, data)
        render_template("qml_group.cpp.j2", (quick_output_directory / (type_name + "Group")).with_suffix(".cpp"), jinja_env, data)

    data = {"types": types.values()}

    render_template("formatter.h.j2", src_directory / "Formatter.h", jinja_env, data)
    render_template("CreateTestInstances.h.j2", tests_directory / "CreateTestInstances.h", jinja_env, data)
    render_template("CMakeLists.txt.j2", src_directory / "CMakeLists.txt", jinja_env, {"target_name": "Union", "file_suffix": ""} | data)
    render_template("CMakeLists.tests.txt.j2", tests_directory / "CMakeLists.txt", jinja_env, data)
    render_template("CMakeLists.txt.j2", quick_output_directory / "CMakeLists.txt", jinja_env, {"target_name": "UnionQuickImpl", "file_suffix": "Group"} | data)

    render_template("properties.css.j2", css_generated_path, jinja_env, data)
