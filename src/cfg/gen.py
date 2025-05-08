#!/usr/bin/env python3

import json
import os

def generate_key(fd, fields):
    fd.write("enum class key : uint32_t{\n")
    for field in fields:
        fd.write(f"    {field['name']} = {field['id']},\n")
    fd.write("};\n")

def generate_key_values(fd, fields):
    fd.write("constexpr std::array<key, ")
    fd.write(f"{len(fields)}")
    fd.write("> keys = {")
    for field in fields:
        fd.write(f"key::{field['name']}, ")
    fd.write("};\n")

def generate_ids(fd, fields):
    fd.write("constexpr std::array<uint32_t, ")
    fd.write(f"{len(fields)}")
    fd.write("> ids = {")
    for field in fields:
        fd.write(f"{field['id']}, ")
    fd.write("};\n")

def generate_str_to_key(fd, fields):
    fd.write("constexpr std::optional<key> str_to_key(std::string_view str){\n")
    for field in fields:
        fd.write(f"    if(str == \"{field['name']}\")\n")
        fd.write(f"        return key::{field['name']};\n")
    fd.write("    return {};\n")
    fd.write("}\n")

def generate_key_to_str(fd, fields):
    fd.write("constexpr std::string_view key_to_str(key k){\n")
    for field in fields:
        fd.write(f"    if(k == key::{field['name']})\n")
        fd.write(f"        return \"{field['name']}\";\n")
    fd.write("    return {};\n")
    fd.write("}\n")

def generate_map(fd, fields):
    fd.write("struct map{\n")
    for field in fields:
        if field['desc'] != "":
            fd.write(f"    // {field['desc']}\n")
        fd.write(f"    {field['type']} {field['name']} = {field['default']};\n")
    fd.write("template<key K> constexpr auto& ref_by_key(){\n")
    for field in fields:
        fd.write(f"    if constexpr(K == key::{field['name']})\n")
        fd.write(f"        return {field['name']};\n")
    fd.write("}\n")
    for x in ["", "const "]:
        fd.write(f"constexpr vari::vptr<value_type {x}> ref_by_id(uint32_t id){x}{{\n")
        for field in fields:
            fd.write(f"    if(id == {field['id']})\n")
            fd.write(f"        return &{field['name']};\n")
        fd.write("    return {};\n")
        fd.write("}\n")
    fd.write("};\n")

def generate_cfg(fd, fields):
    generate_key(fd, fields)
    generate_key_values(fd, fields)
    generate_str_to_key(fd, fields)
    generate_key_to_str(fd, fields)
    generate_ids(fd, fields)
    generate_map(fd, fields)


with open('def.json', 'r') as fd:
    fields = json.load(fd)["fields"]

with open('def.hpp', 'r')  as fd:
    hpp_lines = fd.readlines()

copy_text = True
with open('def.hpp', 'w') as fd:
    for line in hpp_lines:
        if copy_text:
            fd.write(line)
            if line.startswith('// GEN BEGIN HERE'):
                copy_text = False
                fd.write("// This part of file is auto-generated. Do not edit.\n")
                generate_cfg(fd, fields)
        else:
            if line.startswith('// GEN END HERE'):
                copy_text = True
                fd.write(line)

os.system("clang-format -i def.hpp")
