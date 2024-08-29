#!/usr/bin/env python

import dataclasses
from pathlib import Path
import shutil

import yaml
import jinja2

base_directory = Path(__file__).parent
root_directory = base_directory.parent.parent
src_directory = root_directory / "src" / "properties"
tests_directory = root_directory / "autotests" / "properties"
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


class PreserveAliasesLoader(yaml.SafeLoader):
    def __init__(self, stream):
        super().__init__(stream)

    def compose_node(self, parent, index):
        if self.check_event(yaml.AliasEvent):
            event = self.get_event()
            return AliasNode("", event.anchor, event.start_mark, event.end_mark)

        event = self.peek_event()
        result = super().compose_node(parent, index)

        if event.anchor is not None:
            result.anchor = event.anchor
        else:
            result.anchor = None

        return result


@dataclasses.dataclass
class ValueDescription:
    name: str
    type: str = ""
    is_property: bool = False


@dataclasses.dataclass
class PropertyDescription:
    type_name: str
    values: list[ValueDescription] = dataclasses.field(default_factory=list)
    system_includes: set[str] = dataclasses.field(default_factory=set)
    local_includes: set[str] = dataclasses.field(default_factory=set)

    def __lt__(self, other):
        return self.type_name < other.type_name


def ucfirst(value):
    return f"{value[0].upper()}{value[1:]}"


def qualified_name(type_name):
    return f"{ucfirst(type_name)}Property"


def process_node(node, type_name, memo):
    if not isinstance(node, yaml.MappingNode):
        raise RuntimeError(f"Node {node} is not a mapping node!")

    description = PropertyDescription(type_name)
    memo[type_name] = description

    for key_node, value_node in node.value:
        value = ValueDescription(key_node.value)
        match type(value_node).__name__:
            case "MappingNode":
                type_name = qualified_name(value_node.anchor) if value_node.anchor is not None else qualified_name(key_node.value)
                memo = memo | process_node(value_node, type_name, memo)

                value.type = type_name
                value.is_property = True

            case "AliasNode":
                sub_property = memo[qualified_name(value_node.value)]

                value.type = sub_property.type_name
                value.is_property = True

            case "ScalarNode":
                value.type = value_node.value

        for entry in include_patterns:
            if not value.type.startswith(entry["pattern"]):
                continue

            system = entry.get("system_include", False)
            include = entry.get("use_include", value.type if system else value.type + ".h")

            if include is None:
                break

            if system:
                description.system_includes.add(include)
            else:
                description.local_includes.add(include)

            break

        description.values.append(value)

    return memo


if __name__ == "__main__":
    structure = None
    with open(base_directory / "properties.yml") as f:
        structure = yaml.compose(f, PreserveAliasesLoader)

    types = process_node(structure, "StyleProperty", {})

    jinja_env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(base_directory),
        autoescape=False,
        trim_blocks=True,
    )
    jinja_env.filters["ucfirst"] = ucfirst

    shutil.rmtree(src_directory, ignore_errors = True)
    shutil.rmtree(tests_directory, ignore_errors = True)
    shutil.rmtree(quick_output_directory, ignore_errors = True)

    src_directory.mkdir(exist_ok = True)
    tests_directory.mkdir(exist_ok = True)
    quick_output_directory.mkdir(exist_ok = True)

    for type_name, type_definition in types.items():
        data = dataclasses.asdict(type_definition)

        with open((src_directory / type_name).with_suffix(".h"), "w") as f:
            template = jinja_env.get_template("property.h.j2")
            f.write(template.render(data))

        with open((src_directory / type_name).with_suffix(".cpp"), "w") as f:
            template = jinja_env.get_template("property.cpp.j2")
            f.write(template.render(data))

        with open((tests_directory / ("Test" + type_name)).with_suffix(".cpp"), "w") as f:
            template = jinja_env.get_template("autotest.cpp.j2")
            f.write(template.render(data))

        with open((quick_output_directory / (type_name + "Group")).with_suffix(".h"), "w") as f:
            template = jinja_env.get_template("qml_group.h.j2")
            f.write(template.render(data))

        with open((quick_output_directory / (type_name + "Group")).with_suffix(".cpp"), "w") as f:
            template = jinja_env.get_template("qml_group.cpp.j2")
            f.write(template.render(data))

    data = {"types": types.values()}

    with open(src_directory / "Formatter.h", "w") as f:
        template = jinja_env.get_template("formatter.h.j2")
        f.write(template.render(data))

    with open(tests_directory / "CreateTestInstances.h", "w") as f:
        template = jinja_env.get_template("CreateTestInstances.h.j2")
        f.write(template.render(data))

    with open(src_directory / "CMakeLists.txt", "w") as f:
        template = jinja_env.get_template("CMakeLists.txt.j2")
        f.write(template.render({"target_name": "Union", "file_suffix": ""} | data))

    with open(tests_directory / "CMakeLists.txt", "w") as f:
        template = jinja_env.get_template("CMakeLists.tests.txt.j2")
        f.write(template.render(data))

    with open(quick_output_directory / "CMakeLists.txt", "w") as f:
        template = jinja_env.get_template("CMakeLists.txt.j2")
        f.write(template.render({"target_name": "UnionQuickImpl", "file_suffix": "Group"} | data))
