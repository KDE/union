#!/usr/bin/env python

# SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
# SPDX-FileCopyrightText: 2024 Arjen Hiemstra <ahiemstra@heimr.nl>

import copy
import dataclasses
from pathlib import Path
import shutil
from typing import Optional, Type
import re

import ruamel.yaml as yaml
import jinja2

from yaml_helpers import AliasNode, PreserveAliasesComposer, mapping_value


base_directory = Path(__file__).parent
template_directory = base_directory / "templates"
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

# A mapping to map C++ types to CSS syntax
css_type_map = {
    "bool": "true | false",
    "Union::Color": "<color>",
    "qreal": "<length>",
    "int": "<integer>",
    "QString": "<string>",
    "QUrl": "<url>",
    "std::filesystem::path": "<url>",
    "Union::Properties::AlignmentContainer": "item | content | background",
    "Union::Properties::Alignment": "start | center | end | fill | stack-center | stack-fill",
    "Union::Properties::LineStyle": "none | solid",
    "Union::Properties::TextWrapMode": "no-wrap | word-wrap | manual-wrap | wrap-anywhere | wrap-at-word-boundary-or-anywhere | wrap",
    "Union::Properties::TextElide": "none | left | middle | right",
}


@dataclasses.dataclass
class CssDescription:
    implicit_name: str = ""
    explicit_name: str = ""
    syntax: str = ""
    documentation: str = ""
    ignore: bool = False
    extra_properties: list["CssDescription"] = dataclasses.field(default_factory=list)


@dataclasses.dataclass
class Description:
    name: str
    type: str
    parent: Optional["Description"] = None
    children: list["Description"] = dataclasses.field(default_factory=list)

    system_includes: dict[str, set[str]] = dataclasses.field(default_factory=dict)
    local_includes: dict[str, set[str]] = dataclasses.field(default_factory=dict)
    extra_code: dict[str, str] = dataclasses.field(default_factory=dict)

    documentation: str = ""

    css: CssDescription | None = None

    def __lt__(self, other):
        return self.type < other.type

    def add_system_include(self, file: str, include: str) -> None:
        if file not in self.system_includes:
            self.system_includes[file] = set()
        self.system_includes[file].add(include)

    def add_local_include(self, file: str, include: str) -> None:
        if file not in self.local_includes:
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


def implicit_css_name(description):
    parts = []
    while description:
        css = description.css
        if css and css.ignore:
            pass
        elif css and css.explicit_name:
            parts.append(css.explicit_name)
            break
        else:
            parts.append(description.name)
        description = description.parent

    return "-".join(reversed(parts))


def process_css_node(node, description):
    if not isinstance(node, yaml.MappingNode):
        raise RuntimeError(f"Node {node} is not a mapping node!")

    if description is None:
        description = CssDescription()

    for key_node, value_node in node.value:
        if key_node.value == "name":
            description.explicit_name = value_node.value
            description.implicit_name = value_node.value

        if key_node.value == "syntax":
            description.syntax = value_node.value

        if key_node.value == "doc":
            description.documentation = value_node.value

        if key_node.value == "ignore":
            description.ignore = value_node.value

        if key_node.value == "extra_properties":
            for entry in value_node.value:
                description.extra_properties.append(process_css_node(entry, None))

    return description


def process_node(node, name: str, parent: Description, memo: dict[str, Description], type_name: str | None = None):
    if not isinstance(node, yaml.MappingNode):
        raise RuntimeError(f"Node {node} is not a mapping node!")

    node_type = mapping_value(node, "type")
    if node_type is None:
        raise RuntimeError(f"Node {node} is missing a type!")

    if not type_name:
        type_name = node_type

    description_type = node_type
    if node_type == "group" or node_type == "type":
        description_type = group_name(type_name)

    description = None
    if description_type in memo:
        description = copy.deepcopy(memo[description_type])
        description.name = name
        description.parent = parent
    else:
        description = Description(name, description_type, parent)
        if node_type == "group" or node_type == "type":
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
            description.documentation = value_node.value

        elif key_node.value == "types":
            for key_node, value_node in value_node.value:
                memo = memo | process_node(value_node, key_node.value, description, memo, type_name = key_node.value)

        elif key_node.value == "css":
            description.css = process_css_node(value_node, description.css)

        elif key_node.value == "children":
            for key_node, value_node in value_node.value:
                if isinstance(value_node, AliasNode):
                    child = copy.deepcopy(memo[value_node.value])
                    child.name = key_node.value
                    child.parent = description
                    description.add_child(child)
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

    if description.css is None:
        description.css = CssDescription()

    if not description.css.syntax:
        description.css.syntax = css_type_map.get(description.type, "")

    if not description.css.documentation:
        description.css.documentation = description.documentation

    if parent and node_type != "type":
        parent.add_child(description)

    return memo


def postprocess_css_names(types):
    for type in types:
        type.css.implicit_name = implicit_css_name(type)

        if "{prefix}" in type.css.implicit_name:
            type.css.implicit_name = type.css.implicit_name.replace("{prefix}", type.parent.css.implicit_name)

        for p in type.css.extra_properties:
            if "{prefix}" in p.implicit_name:
                p.implicit_name = p.implicit_name.replace("{prefix}", type.css.implicit_name)

        postprocess_css_names(type.children)


@jinja2.pass_context
def render_template_filter(context, value, **kwargs):
    return context.environment.from_string(value).render(context, **kwargs)


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
    parser.Composer = PreserveAliasesComposer

    with open(base_directory / "properties.yml") as f:
        structure = parser.compose(f)

    types = process_node(structure, "style", None, {}, "style")

    postprocess_css_names(types.values())

    jinja_env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(template_directory),
        autoescape=False,
        trim_blocks=True,
        lstrip_blocks=True,
    )
    jinja_env.filters["ucfirst"] = ucfirst
    jinja_env.filters["render"] = render_template_filter

    shutil.rmtree(src_directory, ignore_errors = True)
    shutil.rmtree(tests_directory, ignore_errors = True)
    shutil.rmtree(quick_output_directory, ignore_errors = True)

    css_generated_path = css_input_directory / "generated-properties.css"
    if css_generated_path.exists():
        css_generated_path.unlink()

    css_doc_generated_path = css_docs_directory / "css-properties.qdoc"
    if css_doc_generated_path.exists():
        css_doc_generated_path.unlink()

    src_directory.mkdir(exist_ok = True)
    tests_directory.mkdir(exist_ok = True)
    css_input_directory.mkdir(exist_ok = True)
    quick_output_directory.mkdir(exist_ok = True)

    for name, type_definition in types.items():
        data = {field.name: getattr(type_definition, field.name) for field in dataclasses.fields(type_definition)}

        type_name = type_definition.type

        render_template("property.h.j2", (src_directory / type_name).with_suffix(".h"), jinja_env, data)
        render_template("property.cpp.j2", (src_directory / type_name).with_suffix(".cpp"), jinja_env, data)

        render_template("autotest.cpp.j2", (tests_directory / ("Test" + type_name)).with_suffix(".cpp"), jinja_env, data)

        render_template("qml_group.h.j2", (quick_output_directory / (type_name + "Quick")).with_suffix(".h"), jinja_env, data)
        render_template("qml_group.cpp.j2", (quick_output_directory / (type_name + "Quick")).with_suffix(".cpp"), jinja_env, data)

    data = {"types": types.values()}

    render_template("CreateTestInstances.h.j2", tests_directory / "CreateTestInstances.h", jinja_env, data)
    render_template("CMakeLists.txt.j2", src_directory / "CMakeLists.txt", jinja_env, {"target_name": "Union", "file_suffix": ""} | data)
    render_template("CMakeLists.tests.txt.j2", tests_directory / "CMakeLists.txt", jinja_env, data)
    render_template("CMakeLists.txt.j2", quick_output_directory / "CMakeLists.txt", jinja_env, {"target_name": "UnionQuickImpl", "file_suffix": "Quick"} | data)

    render_template("properties.css.j2", css_generated_path, jinja_env, data)
    render_template("css-properties.qdoc.j2", css_doc_generated_path, jinja_env, data)
