#!/usr/bin/env python3

import json
import argparse
import os
from typing import Dict, List, Any, Callable, TextIO, Optional

# Type aliases for better readability
CmdHandler = Callable[[TextIO, Dict[str, Any], List[Dict[str, Any]], str], None]
GroupHandler = Callable[[TextIO, Dict[str, Any], str], None]


def apply_prefix(prefix: str, name: str, sep: str = "_") -> str:
    if prefix != "":
        return prefix + sep + name
    return name


def gen_arg_cmd_struct(
    fd: TextIO, cmd: Dict[str, Any], args: List[Dict[str, Any]], prefix: str
) -> None:
    name = apply_prefix(prefix, cmd["name"])
    fd.write(f"struct {name}_stmt {{\n")
    for arg in args:
        fd.write(f" {arg['type']} {arg['name']}")
        if "default" in arg:
            fd.write(f" = {arg['default']}")
        else:
            fd.write(" = {}")
        fd.write(";\n")
    fd.write(
        f"friend constexpr auto operator<=>(const {name}_stmt&, const {name}_stmt&) noexcept = default;\n"
    )
    fd.write("};\n")


def gen_arg_cmd(
    fd: TextIO, cmd: Dict[str, Any], args: List[Dict[str, Any]], prefix: str
) -> None:
    name = apply_prefix(prefix, cmd["name"])
    n = len(args)

    fd.write(
        f"""
    static std::tuple<{name}_stmt, parse_status> _{name}(arg_parser ap) {{
    """
    )

    basic_types = ["string", "int32_t", "float", "bool", "expr_tok"]

    arg_def = []
    for arg in args:
        st = "arg_status::MISSING" if "default" not in arg else "arg_status::DEFAULT"
        kw = f'"{arg["name"]}"'
        if arg["type"] in basic_types:
            val = f"res.{arg['name']}"
        else:
            fd.write(f"_convert_type<{arg['type']}>::type {arg['name']};")
            val = f"{arg['name']}"
        arg_def.append(f"arg_def{{ .st = {st}, .kw = {kw}, .val = {val} }}")

    fd.write(
        f"""
        {name}_stmt res;
        std::array<arg_def, {n}> arg_defs = {{{",".join(arg_def)}}};
        parse_status st = std::move(ap).parse_args(arg_defs);
    """
    )

    for arg in args:
        if arg["type"] in basic_types:
            continue
        fd.write(
            f"""
        if(st == parse_status::SUCCESS)
            st = _convert({arg["name"]}, res.{arg["name"]});
                 """
        )

    fd.write(
        f"""
        return {{std::move(res), st}};
    }}\n
    """
    )


def gen_arg_doc(
    fd: TextIO, cmd: Dict[str, Any], args: List[Dict[str, Any]], prefix: str
) -> None:
    name = apply_prefix(prefix, cmd["name"], " ")
    level = name.count(" ") * "#" + "##"
    fd.write(f"{level} cmd: {name}")
    for arg in args:
        fd.write(f" <{arg['name']}")
        if "default" in arg:
            fd.write(f" = {arg['default']}")
        fd.write(">")
    fd.write("\n\n")
    fd.write(f"{cmd['desc']}\n\n")
    for arg in args:
        fd.write(f"- _{arg['name']}_: {arg['desc']}\n")
        fd.write(f"  - type: _{arg['type']}_\n")
        if "unit" in arg:
            fd.write(f"  - unit: _{arg['unit']}_\n")


def gen_group_cmd_struct(fd: TextIO, cmd: Dict[str, Any], prefix: str) -> None:
    name = apply_prefix(prefix, cmd["name"])
    if name != "":
        name += "_"
    if "type" in cmd and cmd["type"] == "forward_group":
        fd.write(
            f"""
        struct {name}forward {{
            std::string_view cmd;
            cmd_parser parser;
            friend constexpr auto operator<=>(const {name}forward&, const {name}forward&) noexcept = default;
        }};
        """
        )
    sub_types = [name + subcmd["name"] + "_stmt" for subcmd in cmd["cmds"]]
    if "type" in cmd and cmd["type"] == "forward_group":
        sub_types.append(name + "forward")
    fd.write(
        f"""
        using {name}stmts = vari::typelist<{",".join(sub_types)}>;
    """
    )
    fd.write(
        f"""struct {name}stmt {{
        vari::vval<{name}stmts> sub = {sub_types[0]}{{}};

        friend constexpr auto operator<=>(const {name}stmt&, const {name}stmt&) noexcept = default;
    }};"""
    )


def gen_group_cmd(fd: TextIO, cmd: Dict[str, Any], prefix: str) -> None:
    name = apply_prefix(prefix, cmd["name"])
    if name != "":
        fun_name = "_" + name
        res_name = name + "_stmt"
        sub_name = "_" + name
    else:
        fun_name = "_root"
        res_name = "stmt"
        sub_name = ""
    fd.write(
        f"""
    static std::tuple<{res_name}, parse_status> {fun_name}(cmd_parser p) {{
        {res_name} res;
        auto id = p.next_cmd();
        parse_status st = parse_status::UNKNOWN_CMD;
        if (!id) {{
            return {{std::move(res), parse_status::CMD_MISSING}};
        }}"""
    )
    for subcmd in cmd["cmds"]:
        fd.write(
            f"""
        else if(*id == "{subcmd['name']}"){{
            std::tie(res.sub, st) = {sub_name}_{subcmd['name']}(std::move(p));
        }}"""
        )
    if "type" in cmd and cmd["type"] == "forward_group":
        fd.write(
            f"""
        else {{
            st = parse_status::SUCCESS;
            res.sub = {name}_forward{{
                .cmd = *id,
                .parser = std::move(p),
            }};
        }}"""
        )
    else:
        fd.write(
            f"""
            else return {{std::move(res), parse_status::UNKNOWN_CMD}};
        """
        )
    fd.write("return { std::move(res), st }; }")


def gen_group_doc(fd: TextIO, cmd: Dict[str, Any], prefix: str) -> None:
    if cmd["name"] == "":
        return
    name = apply_prefix(prefix, cmd["name"], " ")
    level = name.count(" ") * "#" + "##"
    fd.write(f"{level} group: {name}\n\n")
    fd.write(f"{cmd['desc']}\n\n")


def traverse_commands(
    fd: TextIO,
    cmd: Dict[str, Any],
    prefix: str,
    cmd_handler: CmdHandler,
    group_handler: GroupHandler,
    separator: str = "_",
    group_first: bool = False,
) -> None:
    """Generic function to traverse command structure with different handlers."""
    p = apply_prefix(prefix, cmd["name"], separator)

    if group_first:
        group_handler(fd, cmd, prefix)

    for subcmd in cmd["cmds"]:
        if "type" not in subcmd:
            cmd_handler(fd, subcmd, [], p)
        elif subcmd["type"] == "group" or subcmd["type"] == "forward_group":
            traverse_commands(
                fd, subcmd, p, cmd_handler, group_handler, separator, group_first
            )
        elif subcmd["type"] == "arg":
            cmd_handler(fd, subcmd, subcmd["args"], p)
        else:
            raise ValueError(f"Unknown command type: {subcmd['type']}")

    if not group_first:
        group_handler(fd, cmd, prefix)


def traverse_gen(fd: TextIO, cmd: Dict[str, Any], prefix: str) -> None:
    """Generate parsing code by traversing commands."""
    traverse_commands(fd, cmd, prefix, gen_arg_cmd, gen_group_cmd)


def traverse_structs(fd: TextIO, cmd: Dict[str, Any], prefix: str) -> None:
    """Generate struct definitions by traversing commands."""
    traverse_commands(fd, cmd, prefix, gen_arg_cmd_struct, gen_group_cmd_struct)


def traverse_doc(fd: TextIO, cmd: Dict[str, Any], prefix: str) -> None:
    """Generate documentation by traversing commands."""
    traverse_commands(
        fd, cmd, prefix, gen_arg_doc, gen_group_doc, " ", group_first=True
    )


def gen_type(fd: TextIO, tp: Dict[str, Any]) -> None:
    if tp["kind"] == "enum":
        fd.write(f"enum class {tp['name']} {{\n")
        for value in tp["values"]:
            fd.write(f"    {value['key']}, // {value['desc']}\n")
        fd.write("};\n\n")

        fd.write(
            f"""
        static constexpr std::array< {tp['name']}, {len(tp['values'])}> {tp['name']}_values = {{
        """
        )
        for value in tp["values"]:
            fd.write(f"            {tp['name']}::{value['key']},\n")
        fd.write("};")

        fd.write(
            f"""
        static constexpr std::string_view to_str({tp['name']} v) {{
            switch(v) {{
        """
        )
        for value in tp["values"]:
            fd.write(
                f"                    case {tp['name']}::{value['key']}: return \"{value['key']}\";\n"
            )
        fd.write('} return "unknown";} ')

    else:
        raise ValueError(f"Unknown type: {tp['kind']} in {tp['name']}")


def gen_type_convert(fd: TextIO, tp: Dict[str, Any]) -> None:
    if tp["kind"] == "enum":
        fd.write(
            f"""
        template<> struct _convert_type_impl<{tp['name']}> {{
                        using   type = string;
        }};
        constexpr parse_status _convert(string const& src, {tp['name']}& dst) {{
        """
        )
        for value in tp["values"]:
            fd.write(
                f"""
            if(src == "{value['key']}") {{
                dst = {tp['name']}::{value['key']};
                return parse_status::SUCCESS;
            }}"""
            )
        fd.write("return parse_status::UNKNOWN_VALUE;\n}")
    else:
        raise ValueError(f"Unknown type: {tp['kind']} in {tp['name']}")


def gen_type_doc(fd: TextIO, tp: Dict[str, Any]) -> None:
    if tp["kind"] == "enum":
        fd.write(f"### enum {tp['name']}\n\n")
        for value in tp["values"]:
            fd.write(f" - _{value['key']}_: ")
            fd.write(f"{value['desc']}\n")
        fd.write("\n")
    else:
        raise ValueError(f"Unknown type: {tp['kind']} in {tp['name']}")


def regenerate(filename: str, cb: Callable[[TextIO], None]) -> None:
    with open(filename, "r") as fd:
        lines = fd.readlines()

    copy_text = True
    with open(filename, "w") as fd:
        for line in lines:
            if copy_text:
                fd.write(line)
                if "GEN BEGIN HERE" in line:
                    copy_text = False
                    cb(fd)
                    fd.write("\n")
            else:
                if "GEN END HERE" in line:
                    copy_text = True
                    fd.write(line)
    os.system(f"clang-format -i {filename}")


def gen_parse(fd: TextIO, root: Dict[str, Any]) -> None:
    for tp in root["types"]:
        gen_type_convert(fd, tp)
    traverse_gen(fd, root, prefix="")


def gen_def(fd: TextIO, root: Dict[str, Any]) -> None:
    for tp in root["types"]:
        gen_type(fd, tp)
    traverse_structs(fd, root, prefix="")


def gen_md(fd: TextIO, root: Dict[str, Any]) -> None:
    for tp in root["types"]:
        gen_type_doc(fd, tp)

    fd.write("Commands \n")
    fd.write("======== \n")
    traverse_doc(fd, root, prefix="")


def main(definition, hdr, parser, doc):
    with open(definition, "r") as fd:
        root = json.load(fd)
        root["name"] = ""

    regenerate(parser, lambda fd: gen_parse(fd, root))
    regenerate(hdr, lambda fd: gen_def(fd, root))
    regenerate(doc, lambda fd: gen_md(fd, root))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate interface files from JSON definitions."
    )
    parser.add_argument(
        "--definition", type=str, help="Path to the JSON definition file."
    )
    parser.add_argument("--hdr", type=str, help="Path to the output markdown file.")
    parser.add_argument("--parser", type=str, help="Path to the output parser file.")
    parser.add_argument(
        "--doc", type=str, help="Path to the output documentation file."
    )
    args = parser.parse_args()
    main(args.definition, args.hdr, args.parser, args.doc)
