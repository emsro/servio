/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "servio", "index.html", [
    [ "Disclaimer", "index.html#autotoc_md1", null ],
    [ "Dependencies", "index.html#autotoc_md2", null ],
    [ "Build", "index.html#autotoc_md3", null ],
    [ "Flashing", "index.html#autotoc_md4", null ],
    [ "Usage", "index.html#autotoc_md5", null ],
    [ "Config fields", "md_src_2cfg_2cfg.html", [
      [ "1 model", "md_src_2cfg_2cfg.html#autotoc_md7", null ],
      [ "2 id", "md_src_2cfg_2cfg.html#autotoc_md8", null ],
      [ "3 group_id", "md_src_2cfg_2cfg.html#autotoc_md9", null ],
      [ "4 encoder_mode", "md_src_2cfg_2cfg.html#autotoc_md10", null ],
      [ "11 position_low_angle", "md_src_2cfg_2cfg.html#autotoc_md11", null ],
      [ "12 position_high_angle", "md_src_2cfg_2cfg.html#autotoc_md12", null ],
      [ "14 current_conv_scale", "md_src_2cfg_2cfg.html#autotoc_md13", null ],
      [ "15 current_conv_offset", "md_src_2cfg_2cfg.html#autotoc_md14", null ],
      [ "16 temp_conv_scale", "md_src_2cfg_2cfg.html#autotoc_md15", null ],
      [ "17 temp_conv_offset", "md_src_2cfg_2cfg.html#autotoc_md16", null ],
      [ "18 voltage_conv_scale", "md_src_2cfg_2cfg.html#autotoc_md17", null ],
      [ "19 invert_hbridge", "md_src_2cfg_2cfg.html#autotoc_md18", null ],
      [ "30 current_loop_p", "md_src_2cfg_2cfg.html#autotoc_md19", null ],
      [ "31 current_loop_i", "md_src_2cfg_2cfg.html#autotoc_md20", null ],
      [ "32 current_loop_d", "md_src_2cfg_2cfg.html#autotoc_md21", null ],
      [ "33 current_lim_min", "md_src_2cfg_2cfg.html#autotoc_md22", null ],
      [ "34 current_lim_max", "md_src_2cfg_2cfg.html#autotoc_md23", null ],
      [ "40 velocity_loop_p", "md_src_2cfg_2cfg.html#autotoc_md24", null ],
      [ "41 velocity_loop_i", "md_src_2cfg_2cfg.html#autotoc_md25", null ],
      [ "42 velocity_loop_d", "md_src_2cfg_2cfg.html#autotoc_md26", null ],
      [ "43 velocity_lim_min", "md_src_2cfg_2cfg.html#autotoc_md27", null ],
      [ "44 velocity_lim_max", "md_src_2cfg_2cfg.html#autotoc_md28", null ],
      [ "45 velocity_to_current_lim_scale", "md_src_2cfg_2cfg.html#autotoc_md29", null ],
      [ "50 position_loop_p", "md_src_2cfg_2cfg.html#autotoc_md30", null ],
      [ "51 position_loop_i", "md_src_2cfg_2cfg.html#autotoc_md31", null ],
      [ "52 position_loop_d", "md_src_2cfg_2cfg.html#autotoc_md32", null ],
      [ "53 position_lim_min", "md_src_2cfg_2cfg.html#autotoc_md33", null ],
      [ "54 position_lim_max", "md_src_2cfg_2cfg.html#autotoc_md34", null ],
      [ "55 position_to_velocity_lim_scale", "md_src_2cfg_2cfg.html#autotoc_md35", null ],
      [ "60 static_friction_scale", "md_src_2cfg_2cfg.html#autotoc_md36", null ],
      [ "61 static_friction_decay", "md_src_2cfg_2cfg.html#autotoc_md37", null ],
      [ "62 minimum_voltage", "md_src_2cfg_2cfg.html#autotoc_md38", null ],
      [ "65 maximum_temperature", "md_src_2cfg_2cfg.html#autotoc_md39", null ],
      [ "66 moving_detection_step", "md_src_2cfg_2cfg.html#autotoc_md40", null ],
      [ "80 quad_encoder_range", "md_src_2cfg_2cfg.html#autotoc_md41", null ]
    ] ],
    [ "Interface", "md_src_2iface_2iface.html", [
      [ "Message", "md_src_2iface_2iface.html#autotoc_md42", null ],
      [ "Reply", "md_src_2iface_2iface.html#autotoc_md43", null ],
      [ "Types", "md_src_2iface_2iface.html#autotoc_md44", [
        [ "int32_t", "md_src_2iface_2iface.html#autotoc_md45", null ],
        [ "float", "md_src_2iface_2iface.html#autotoc_md46", null ],
        [ "string", "md_src_2iface_2iface.html#autotoc_md47", null ],
        [ "bool", "md_src_2iface_2iface.html#autotoc_md48", null ],
        [ "expr_tok", "md_src_2iface_2iface.html#autotoc_md49", null ],
        [ "enum property", "md_src_2iface_2iface.html#autotoc_md50", null ]
      ] ],
      [ "Commands", "md_src_2iface_2iface.html#autotoc_md51", [
        [ "group: mode", "md_src_2iface_2iface.html#autotoc_md52", [
          [ "cmd: mode disengaged", "md_src_2iface_2iface.html#autotoc_md53", null ],
          [ "cmd: mode power <power = 0>", "md_src_2iface_2iface.html#autotoc_md54", null ],
          [ "cmd: mode current <current = 0.0>", "md_src_2iface_2iface.html#autotoc_md55", null ],
          [ "cmd: mode velocity <velocity = 0.0>", "md_src_2iface_2iface.html#autotoc_md56", null ],
          [ "cmd: mode position <position = 0.0>", "md_src_2iface_2iface.html#autotoc_md57", null ]
        ] ],
        [ "cmd: prop <name>", "md_src_2iface_2iface.html#autotoc_md58", null ],
        [ "group: cfg", "md_src_2iface_2iface.html#autotoc_md59", [
          [ "cmd: cfg set <field> <value>", "md_src_2iface_2iface.html#autotoc_md60", null ],
          [ "cmd: cfg get <field>", "md_src_2iface_2iface.html#autotoc_md61", null ],
          [ "cmd: cfg list5 <level = \"\"> <offset = 0> <n = 5>", "md_src_2iface_2iface.html#autotoc_md62", null ],
          [ "cmd: cfg commit", "md_src_2iface_2iface.html#autotoc_md63", null ],
          [ "cmd: cfg clear", "md_src_2iface_2iface.html#autotoc_md64", null ]
        ] ],
        [ "cmd: info", "md_src_2iface_2iface.html#autotoc_md65", null ]
      ] ]
    ] ],
    [ "Motor related math", "md_doc_2math.html", [
      [ "Dictionary", "md_doc_2math.html#autotoc_md67", null ],
      [ "Equations", "md_doc_2math.html#autotoc_md68", [
        [ "velocity-torque curve", "md_doc_2math.html#autotoc_md69", null ]
      ] ]
    ] ],
    [ "Requirements", "md_doc_2requirements.html", [
      [ "System requirements", "sys.html", [
        [ "REQ1.1 Prioritize project independence to hardware", "sys.html#autotoc_md70", [
          [ "REQ1.1.1 Decomposition", "sys.html#autotoc_md71", null ],
          [ "REQ1.1.2 Code shall not be dependent on hardware specifics unless necessary", "sys.html#autotoc_md72", null ]
        ] ],
        [ "REQ1.2 Focus on testing", "sys.html#autotoc_md73", null ],
        [ "REQ1.2.1 Test on host what can be tested on host", "sys.html#autotoc_md74", null ],
        [ "REQ1.2.3 Use testing on target hardware", "sys.html#autotoc_md75", null ]
      ] ],
      [ "System control requirements", "ctl.html", [
        [ "REQ2.1 Position control mode", "ctl.html#autotoc_md76", null ],
        [ "REQ2.2 Current control mode", "ctl.html#autotoc_md77", null ],
        [ "REQ2.3 Velocity control mode", "ctl.html#autotoc_md78", null ],
        [ "REQ2.4 Power control mode", "ctl.html#autotoc_md79", null ],
        [ "REQ2.5 Movement detection", "ctl.html#autotoc_md80", null ],
        [ "REQ2.6 Static friction compensation", "ctl.html#autotoc_md81", null ],
        [ "REQ2.7 Disengaged", "ctl.html#autotoc_md82", null ]
      ] ],
      [ "System limits requirements", "lims.html", [
        [ "REQ3.1 Current limits", "lims.html#autotoc_md83", null ],
        [ "REQ3.2 Position limits", "lims.html#autotoc_md84", null ],
        [ "REQ3.3 Velocity limits", "lims.html#autotoc_md85", null ]
      ] ],
      [ "Communication requirements", "comms.html", [
        [ "REQ4.1 System should have one major communication interface", "comms.html#autotoc_md86", null ],
        [ "REQ4.2 System should decompose message communication to be independent on underlying transport layer", "comms.html#autotoc_md87", null ],
        [ "RE4.3 UART shall be supported as transport", "comms.html#autotoc_md88", null ],
        [ "REQ4.4 Major communication message shall be defined in TBD", "comms.html#autotoc_md89", null ],
        [ "REQ4.5 COBS can be used as framing mechanism", "comms.html#autotoc_md90", null ],
        [ "REQ4.6 Exact communication interface is property of board", "comms.html#autotoc_md91", null ],
        [ "REQ4.7 ID", "comms.html#autotoc_md92", null ],
        [ "REQ4.8 Group ID", "comms.html#autotoc_md93", null ]
      ] ],
      [ "Units", "units.html", [
        [ "REQ5.1 Any values facing the user shall be in well defined units specified by other requirements", "units.html#autotoc_md94", null ],
        [ "REQ5.2 Angular position is specified in radians", "units.html#autotoc_md95", null ],
        [ "REQ5.3 Electric current is specified in amperes", "units.html#autotoc_md96", null ],
        [ "REQ5.4 Time is specified in seconds", "units.html#autotoc_md97", null ],
        [ "REQ5.5 Angular velocity is specified in radians per second.", "units.html#autotoc_md98", null ]
      ] ],
      [ "Configuration", "config.html", [
        [ "REQ6.1 One configuration", "config.html#autotoc_md99", null ],
        [ "REQ6.2 Configuration can be managed by the user via the communication interface", "config.html#autotoc_md100", [
          [ "REQ6.2.1 Get configuration field", "config.html#autotoc_md101", null ],
          [ "REQ6.2.2 Set configuration field", "config.html#autotoc_md102", null ],
          [ "REQ6.2.3 Commit the configuration", "config.html#autotoc_md103", null ],
          [ "REQ6.2.4 Clear the configuration", "config.html#autotoc_md104", null ]
        ] ],
        [ "REQ6.3 Configuration shall be documented", "config.html#autotoc_md105", null ],
        [ "REQ6.4 Configuration is tied to the version of firmware", "config.html#autotoc_md106", null ],
        [ "REQ6.5 Persistent configuration", "config.html#autotoc_md107", [
          [ "REQ6.5.1 Entire set is always stored", "config.html#autotoc_md108", null ],
          [ "REQ6.5.2 Size of history", "config.html#autotoc_md109", null ],
          [ "REQ6.5.3 Clear removes history", "config.html#autotoc_md110", null ],
          [ "REQ6.5.4 Firmware version is also stored", "config.html#autotoc_md111", null ]
        ] ],
        [ "REQ6.6 Firmware version changes", "config.html#autotoc_md112", null ],
        [ "REQ6.7 Utility for changes shall exist", "config.html#autotoc_md113", null ],
        [ "REQ6.8 Common format for storing on host side", "config.html#autotoc_md114", null ],
        [ "REQ6.9 List of fields for configuration", "config.html#autotoc_md115", [
          [ "REQ6.9.1 Model", "config.html#autotoc_md116", null ],
          [ "REQ6.9.2 ID", "config.html#autotoc_md117", null ]
        ] ]
      ] ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", "namespacemembers_dup" ],
        [ "Functions", "namespacemembers_func.html", "namespacemembers_func" ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ],
        [ "Enumerator", "namespacemembers_eval.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", "functions_vars" ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Enumerator", "functions_eval.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"adc_8cpp.html",
"char__uart_8hpp.html",
"cobs__rx__container_8hpp.html#a74cf7569d05ba86cd733572599f74aa4",
"functions_func_o.html",
"md_src_2cfg_2cfg.html#autotoc_md16",
"namespaceservio_1_1brd.html#a6c7d5a095766cdeb38982a8186a2d61f",
"namespaceservio_1_1drv.html#acb96632a153ab3bc0026940e950bb717",
"namespaceservio_1_1plt.html#af748fc48605aa9327b8a17cff5681907",
"preset_8cpp.html#ad9cdcd1956f9f3ad5cbc2f5d1eed77c4",
"str__lib_8hpp.html#a57ef0f842947c125eba788c392bc0722",
"structservio_1_1core_1_1drivers.html#a09f6aabf789b4a4575464e0be3876924",
"structservio_1_1drv_1_1curr__iface.html#a686ea6c4c895c1be5d38c9d2a9857cbd",
"structservio_1_1drv_1_1tests_1_1curr__iface__test.html#a35eefb8d176a9163a319da76e457c05a",
"structservio_1_1ftest_1_1utest.html#a23cd2a14060453b2882ab01f18d8c8a9",
"structservio_1_1mon_1_1blinker.html",
"structservio_1_1scmdio_1_1serial__stream.html#aa022deddb082929fac635cd13b2f3844",
"units.html#autotoc_md96"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';