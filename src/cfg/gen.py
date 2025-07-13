#!/usr/bin/env python3

import json
import os
from typing import Dict, List, Any, TextIO, Callable

def regenerate_file(filename: str, fields: List[Dict[str, Any]],
                   generator_func: Callable[[TextIO, List[Dict[str, Any]]], None]) -> None:
    with open(filename, 'r') as fd:
        lines = fd.readlines()

    with open(filename, 'w') as fd:
        copy_text = True
        for line in lines:
            if copy_text:
                fd.write(line)
                if "GEN BEGIN HERE" in line:
                    copy_text = False
                    generator_func(fd, fields)
            else:
                if "GEN END HERE" in line:
                    copy_text = True
                    fd.write(line)

def generate_key(fd: TextIO, fields: List[Dict[str, Any]]) -> None:
    fd.write("enum class key : uint32_t{\n")
    for field in fields:
        fd.write(f"    {field['name']} = {field['id']},\n")
    fd.write("};\n")

def generate_key_values(fd: TextIO, fields: List[Dict[str, Any]]) -> None:
    fd.write(f"constexpr std::array<key, {len(fields)}> keys = {{")
    for field in fields:
        fd.write(f"key::{field['name']}, ")
    fd.write("};\n")

def generate_ids(fd: TextIO, fields: List[Dict[str, Any]]) -> None:
    fd.write(f"constexpr std::array<uint32_t, {len(fields)}> ids = {{")
    for field in fields:
        fd.write(f"{field['id']}, ")
    fd.write("};\n")

def generate_str_to_key(fd: TextIO, fields: List[Dict[str, Any]]) -> None:
    fd.write("constexpr std::optional<key> str_to_key(std::string_view str) {\n")
    for field in fields:
        fd.write(f"""    if (str == "{field['name']}")
        return key::{field['name']};
""")
    fd.write("    return {};\n}\n")

def generate_to_str(fd: TextIO, fields: List[Dict[str, Any]]) -> None:
    fd.write("constexpr std::string_view to_str(key k) {\n")
    for field in fields:
        fd.write(f"""    if (k == key::{field['name']})
        return "{field['name']}";
""")
    fd.write("    return {};\n}\n")

def generate_key_to_id(fd: TextIO, fields: List[Dict[str, Any]]) -> None:
    fd.write("constexpr uint32_t key_to_id(key k) {\n")
    for field in fields:
        fd.write(f"""    if (k == key::{field['name']})
        return {field['id']};
""")
    fd.write("    return {};\n}\n")

def generate_id_to_key(fd: TextIO, fields: List[Dict[str, Any]]) -> None:
    fd.write("constexpr key id_to_key(uint32_t id) {\n")
    for field in fields:
        fd.write(f"""    if (id == {field['id']})
        return key::{field['name']};
""")
    fd.write("    return {};\n}\n")

def generate_key_trait(fd: TextIO, fields: List[Dict[str, Any]]) -> None:
    fd.write("template<key K> struct key_trait;\n")
    for field in fields:
        fd.write(f"""template<> struct key_trait<key::{field['name']}> {{
    using type = {field['type']};
}};
""")

def generate_map(fd: TextIO, fields: List[Dict[str, Any]]) -> None:
    fd.write("struct map {\n")

    for field in fields:
        if field['desc'] != "":
            fd.write(f"    // {field['desc']}\n")
        fd.write(f"    {field['type']} {field['name']} = {field['default']};\n")

    fd.write("\ntemplate<key K> constexpr auto& ref_by_key() {\n")
    for field in fields:
        fd.write(f"""    if constexpr (K == key::{field['name']})
        return {field['name']};
""")
    fd.write("}\n")

    for const_modifier in ["", "const "]:
        fd.write(f"""
constexpr vari::vptr<value_type {const_modifier}> ref_by_key(key K) {const_modifier}{{
    switch (K) {{
""")
        for field in fields:
            fd.write(f"""    case key::{field['name']}:
        return &{field['name']};
""")
        fd.write(" }\n   return nullptr;\n}\n")

    for const_modifier in ["", "const "]:
        fd.write(f"""
constexpr vari::vptr<value_type {const_modifier}> ref_by_id(uint32_t id) {const_modifier}{{
""")
        for field in fields:
            fd.write(f"""    if (id == {field['id']})
        return &{field['name']};
""")
        fd.write("    return {};\n}\n")

    fd.write("};\n")

def generate_cfg_doc(fd: TextIO, fields: List[Dict[str, Any]]) -> None:

    for field in fields:
        name = field['name']
        field_type = field['type']
        default = field['default']
        unit = field.get('unit', '')
        desc = field.get('desc', '')
        field_id = field['id']

        fd.write(f"""
## {field_id} {name}
{desc}
 - _type_: `{field_type}`
""")
        if unit:
            fd.write(f" - _unit_: {unit}\n")
        if default:
            fd.write(f" - _default_: `{default}`\n")

def generate_cfg(fd: TextIO, fields: List[Dict[str, Any]]) -> None:
    generate_key(fd, fields)
    generate_key_values(fd, fields)
    generate_str_to_key(fd, fields)
    generate_to_str(fd, fields)
    generate_key_to_id(fd, fields)
    generate_id_to_key(fd, fields)
    generate_ids(fd, fields)
    generate_key_trait(fd, fields)
    generate_map(fd, fields)


def main() -> None:
    with open('def.json', 'r') as fd:
        data = json.load(fd)
        if 'fields' not in data:
            print("Error: 'fields' key not found in def.json")
            return
        fields: List[Dict[str, Any]] = data["fields"]

    regenerate_file('def.hpp', fields, generate_cfg)

    regenerate_file('cfg.md', fields, generate_cfg_doc)

    os.system("clang-format -i def.hpp")

if __name__ == '__main__':
    main()
