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

from yaml_helpers import *


base_directory = Path(__file__).parent
root_directory = base_directory.parent.parent
src_directory = root_directory / "src" / "properties"
tests_directory = root_directory / "autotests" / "properties"
css_input_directory = root_directory / "src" / "input" / "css" / "defaults"
css_docs_directory = root_directory / "doc" / "css"
quick_output_directory = root_directory / "src" / "output" / "qtquick" / "plugin" / "properties"

include_patterns = [
    {"pattern": "qreal", "use_include": None},
    {"pattern": "int", "use_include": None},
    {"pattern": "std::filesystem::path", "use_include": "filesystem", "system_include": True},
    {"pattern": "Qt::", "use_include": "QtGlobal", "system_include": True},
    {"pattern": "Q", "system_include": True},
    {"pattern": "Union::Properties::", "use_include": None},
    {"pattern": "Union::Color", "use_include": "../Color.h"},
]


@dataclasses.dataclass
class Description:
    name: str
    type: str
    parent: Optional["Description"] = None
    children: list["Description"] = dataclasses.field(default_factory=list)

    system_includes: dict[str, set[str]] = dataclasses.field(default_factory=dict)
    local_includes: dict[str, set[str]] = dataclasses.field(default_factory=dict)
    extra_code: dict[str, str] = dataclasses.field(default_factory=dict)

    api_documentation: str = ""

    def __lt__(self, other):
        return self.type < other.type

    def add_system_include(self, file: str, include: str) -> None:
        if not file in self.system_includes:
            self.system_includes[file] = set()
        self.system_includes[file].add(include)

    def add_local_include(self, file: str, include: str) -> None:
        if not file in self.local_includes:
            self.local_includes[file] = set()
        self.local_includes[file].add(include)

    def add_child(self, child: "Description") -> None:
        self.children.append(child)
        if child.children:
            self.add_local_include("property.h.j2", child.type + ".h")


def ucfirst(value):
    return f"{value[0].upper()}{value[1:]}"


def group_name(type_name):
    return f"{ucfirst(type_name)}PropertyGroup"


def css_name(name):
    parts = re.split(r"([A-Z][a-z]+)", name)
    result = "-".join(part.lower() for part in parts if part)
    return result.replace("--", "-")


def process_node(node, name: str, parent: Description, memo: dict[str, Description], type_name: str | None = None):
    if not isinstance(node, yaml.MappingNode):
        raise RuntimeError(f"Node {node} is not a mapping node!")

    node_type = mapping_value(node, "type")
    if node_type is None:
        raise RuntimeError(f"Node {node} is missing a type!")

    if not type_name:
        type_name = node_type

    description_type = node_type
    if node_type == "group":
        description_type = group_name(type_name)

    description = Description(name, description_type, parent)
    memo[type_name] = description

    for key_node, value_node in node.value:
        if key_node.value == "extra_code":
            for template_name, extra_code in value_node.value:
                if isinstance(extra_code, yaml.MappingNode):
                    extra_code_data = {}
                    for identifier, code in extra_code.value:
                        extra_code_data[identifier.value] = code.value
                    description.extra_code[template_name.value] = extra_code_data
                else:
                    description.extra_code[template_name.value] = extra_code.value

        elif key_node.value == "extra_system_includes":
            for template_name, includes in value_node.value:
                for include_name in includes.value:
                    description.add_system_include(template_name.value, include_name.value)

        elif key_node.value == "doc":
            if isinstance(value_node, yaml.MappingNode):
                description.api_documentation = mapping_value(value_node, "api", "")
            else:
                description.api_documentation = value_node.value

        elif key_node.value == "children" and node_type == "group":
            for key_node, value_node in value_node.value:
                prop = None

                if isinstance(value_node, AliasNode):
                    group = memo[value_node.value]
                    description.add_child(Description(key_node.value, group.type, description))
                else:
                    child_type_name = value_node.anchor if value_node.anchor is not None else key_node.value
                    memo = memo | process_node(value_node, key_node.value, description, memo, type_name = child_type_name)

    for entry in include_patterns:
        if not description.type.startswith(entry["pattern"]) or not parent:
            continue

        system = entry.get("system_include", False)
        include = entry.get("use_include", description.type if system else description.type + ".h")

        if include is None:
            continue

        if system:
            parent.add_system_include("property.h.j2", include)
        else:
            parent.add_local_include("property.h.j2", include)

    if parent:
        parent.add_child(description)

    return memo


@jinja2.pass_context
def render_template_filter(context, value):
    return context.environment.from_string(value).render(context)


def render_template(template_name: str, output_path: Path, env: jinja2.Environment, data: dict):
    render_data = data.copy()
    render_data["extra_code"] = data.get("extra_code", {}).get(template_name, "")
    render_data["system_includes"] = data.get("system_includes", {}).get(template_name, [])
    render_data["local_includes"] = data.get("local_includes", {}).get(template_name, [])
    render_data["api_documentation"] = data.get("api_documentation", "")

    with open(output_path, "w") as f:
        template = jinja_env.get_template(template_name, None)
        f.write(template.render(render_data))


if __name__ == "__main__":
    structure = None

    parser = yaml.YAML()
    parser.Composer = PreserveAliasesComposer

    with open(base_directory / "properties.yml") as f:
        structure = parser.compose(f)

    types = process_node(structure, "style", None, {}, "style")
    groups = {k: v for (k, v) in types.items() if v.children}

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

    for name, type_definition in groups.items():
        data = {field.name: getattr(type_definition, field.name) for field in dataclasses.fields(type_definition)}

        type_name = type_definition.type

        render_template("property.h.j2", (src_directory / type_name).with_suffix(".h"), jinja_env, data)
        render_template("property.cpp.j2", (src_directory / type_name).with_suffix(".cpp"), jinja_env, data)

        render_template("autotest.cpp.j2", (tests_directory / ("Test" + type_name)).with_suffix(".cpp"), jinja_env, data)

        render_template("qml_group.h.j2", (quick_output_directory / (type_name + "Group")).with_suffix(".h"), jinja_env, data)
        render_template("qml_group.cpp.j2", (quick_output_directory / (type_name + "Group")).with_suffix(".cpp"), jinja_env, data)

    data = {"types": groups.values()}

    render_template("CreateTestInstances.h.j2", tests_directory / "CreateTestInstances.h", jinja_env, data)
    render_template("CMakeLists.txt.j2", src_directory / "CMakeLists.txt", jinja_env, {"target_name": "Union", "file_suffix": ""} | data)
    render_template("CMakeLists.tests.txt.j2", tests_directory / "CMakeLists.txt", jinja_env, data)
    render_template("CMakeLists.txt.j2", quick_output_directory / "CMakeLists.txt", jinja_env, {"target_name": "UnionQuickImpl", "file_suffix": "Group"} | data)

    render_template("properties.css.j2", css_generated_path, jinja_env, data)
