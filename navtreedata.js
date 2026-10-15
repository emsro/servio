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
    [ "Configuration", "index.html#autotoc_md5", null ],
    [ "Communication", "index.html#autotoc_md6", null ],
    [ "Config fields", "md_src_2cfg_2cfg.html", [
      [ "1 model", "md_src_2cfg_2cfg.html#autotoc_md8", null ],
      [ "2 id", "md_src_2cfg_2cfg.html#autotoc_md9", null ],
      [ "3 group_id", "md_src_2cfg_2cfg.html#autotoc_md10", null ],
      [ "4 encoder_mode", "md_src_2cfg_2cfg.html#autotoc_md11", null ],
      [ "11 position_low_angle", "md_src_2cfg_2cfg.html#autotoc_md12", null ],
      [ "12 position_high_angle", "md_src_2cfg_2cfg.html#autotoc_md13", null ],
      [ "14 current_conv_scale", "md_src_2cfg_2cfg.html#autotoc_md14", null ],
      [ "15 current_conv_offset", "md_src_2cfg_2cfg.html#autotoc_md15", null ],
      [ "16 temp_conv_scale", "md_src_2cfg_2cfg.html#autotoc_md16", null ],
      [ "17 temp_conv_offset", "md_src_2cfg_2cfg.html#autotoc_md17", null ],
      [ "18 voltage_conv_scale", "md_src_2cfg_2cfg.html#autotoc_md18", null ],
      [ "19 invert_hbridge", "md_src_2cfg_2cfg.html#autotoc_md19", null ],
      [ "62 minimum_voltage", "md_src_2cfg_2cfg.html#autotoc_md20", null ],
      [ "65 maximum_temperature", "md_src_2cfg_2cfg.html#autotoc_md21", null ],
      [ "66 moving_detection_step", "md_src_2cfg_2cfg.html#autotoc_md22", null ],
      [ "67 moving_detection_window", "md_src_2cfg_2cfg.html#autotoc_md23", null ],
      [ "80 quad_encoder_range", "md_src_2cfg_2cfg.html#autotoc_md24", null ]
    ] ],
    [ "cfg", "md_src_2gov_2current_2cfg.html", null ],
    [ "iface", "md_src_2gov_2current_2iface.html", [
      [ "Commands", "md_src_2gov_2current_2iface.html#autotoc_md33", [
        [ "cmd: set <goal>", "md_src_2gov_2current_2iface.html#autotoc_md34", null ]
      ] ]
    ] ],
    [ "cfg", "md_src_2gov_2position_2cfg.html", null ],
    [ "iface", "md_src_2gov_2position_2iface.html", [
      [ "Commands", "md_src_2gov_2position_2iface.html#autotoc_md48", [
        [ "cmd: set <goal>", "md_src_2gov_2position_2iface.html#autotoc_md49", null ]
      ] ]
    ] ],
    [ "iface", "md_src_2gov_2power_2iface.html", [
      [ "Commands", "md_src_2gov_2power_2iface.html#autotoc_md50", [
        [ "cmd: set <goal>", "md_src_2gov_2power_2iface.html#autotoc_md51", null ]
      ] ]
    ] ],
    [ "cfg", "md_src_2gov_2velocity_2cfg.html", null ],
    [ "iface", "md_src_2gov_2velocity_2iface.html", [
      [ "Commands", "md_src_2gov_2velocity_2iface.html#autotoc_md65", [
        [ "cmd: set <goal>", "md_src_2gov_2velocity_2iface.html#autotoc_md66", null ]
      ] ]
    ] ],
    [ "Interface", "md_src_2iface_2iface.html", [
      [ "Message", "md_src_2iface_2iface.html#autotoc_md67", null ],
      [ "Reply", "md_src_2iface_2iface.html#autotoc_md68", null ],
      [ "Types", "md_src_2iface_2iface.html#autotoc_md69", [
        [ "int32_t", "md_src_2iface_2iface.html#autotoc_md70", null ],
        [ "float", "md_src_2iface_2iface.html#autotoc_md71", null ],
        [ "string", "md_src_2iface_2iface.html#autotoc_md72", null ],
        [ "bool", "md_src_2iface_2iface.html#autotoc_md73", null ],
        [ "expr_tok", "md_src_2iface_2iface.html#autotoc_md74", null ],
        [ "enum property", "md_src_2iface_2iface.html#autotoc_md75", null ]
      ] ],
      [ "Commands", "md_src_2iface_2iface.html#autotoc_md76", [
        [ "group: govctl", "md_src_2iface_2iface.html#autotoc_md81", null ],
        [ "cmd: gov", "md_src_2iface_2iface.html#autotoc_md82", null ],
        [ "cmd: prop <name>", "md_src_2iface_2iface.html#autotoc_md83", [
          [ "cmd: govctl activate <governor>", "md_src_2iface_2iface.html#autotoc_md77", null ],
          [ "cmd: govctl deactivate", "md_src_2iface_2iface.html#autotoc_md78", null ],
          [ "cmd: govctl active", "md_src_2iface_2iface.html#autotoc_md79", null ],
          [ "cmd: govctl list <index>", "md_src_2iface_2iface.html#autotoc_md80", null ],
          [ "cmd: cfg set <field> <value> <governor = \"\">", "md_src_2iface_2iface.html#autotoc_md84", null ],
          [ "cmd: cfg get <field> <governor = \"\">", "md_src_2iface_2iface.html#autotoc_md85", null ],
          [ "cmd: cfg list <index = 0> <governor = \"\">", "md_src_2iface_2iface.html#autotoc_md86", null ],
          [ "cmd: cfg commit", "md_src_2iface_2iface.html#autotoc_md87", null ],
          [ "cmd: cfg clear", "md_src_2iface_2iface.html#autotoc_md88", null ]
        ] ],
        [ "group: cfg", "md_src_2iface_2iface.html#autotoc_md89", null ],
        [ "cmd: info", "md_src_2iface_2iface.html#autotoc_md90", null ]
      ] ]
    ] ],
    [ "Motor related math", "md_doc_2math.html", [
      [ "Dictionary", "md_doc_2math.html#autotoc_md92", null ],
      [ "Equations", "md_doc_2math.html#autotoc_md93", [
        [ "velocity-torque curve", "md_doc_2math.html#autotoc_md94", null ]
      ] ]
    ] ],
    [ "Requirements", "md_doc_2requirements.html", [
      [ "System requirements", "sys.html", [
        [ "REQ1.1 Prioritize project independence to hardware", "sys.html#autotoc_md95", [
          [ "REQ1.1.1 Decomposition", "sys.html#autotoc_md96", null ],
          [ "REQ1.1.2 Code shall not be dependent on hardware specifics unless necessary", "sys.html#autotoc_md97", null ]
        ] ],
        [ "REQ1.2 Focus on testing", "sys.html#autotoc_md98", null ],
        [ "REQ1.2.1 Test on host what can be tested on host", "sys.html#autotoc_md99", null ],
        [ "REQ1.2.3 Use testing on target hardware", "sys.html#autotoc_md100", null ]
      ] ],
      [ "System control requirements", "ctl.html", [
        [ "REQ2.1 Position control mode", "ctl.html#autotoc_md101", null ],
        [ "REQ2.2 Current control mode", "ctl.html#autotoc_md102", null ],
        [ "REQ2.3 Velocity control mode", "ctl.html#autotoc_md103", null ],
        [ "REQ2.4 Power control mode", "ctl.html#autotoc_md104", null ],
        [ "REQ2.5 Movement detection", "ctl.html#autotoc_md105", null ],
        [ "REQ2.6 Static friction compensation", "ctl.html#autotoc_md106", null ],
        [ "REQ2.7 Disengaged", "ctl.html#autotoc_md107", null ]
      ] ],
      [ "System limits requirements", "lims.html", [
        [ "REQ3.1 Current limits", "lims.html#autotoc_md108", null ],
        [ "REQ3.2 Position limits", "lims.html#autotoc_md109", null ],
        [ "REQ3.3 Velocity limits", "lims.html#autotoc_md110", null ]
      ] ],
      [ "Communication requirements", "comms.html", [
        [ "REQ4.1 System should have one major communication interface", "comms.html#autotoc_md111", null ],
        [ "REQ4.2 System should decompose message communication to be independent on underlying transport layer", "comms.html#autotoc_md112", null ],
        [ "RE4.3 UART shall be supported as transport", "comms.html#autotoc_md113", null ],
        [ "REQ4.4 Major communication message shall be defined in TBD", "comms.html#autotoc_md114", null ],
        [ "REQ4.5 COBS can be used as framing mechanism", "comms.html#autotoc_md115", null ],
        [ "REQ4.6 Exact communication interface is property of board", "comms.html#autotoc_md116", null ],
        [ "REQ4.7 ID", "comms.html#autotoc_md117", null ],
        [ "REQ4.8 Group ID", "comms.html#autotoc_md118", null ]
      ] ],
      [ "Units", "units.html", [
        [ "REQ5.1 Any values facing the user shall be in well defined units specified by other requirements", "units.html#autotoc_md119", null ],
        [ "REQ5.2 Angular position is specified in radians", "units.html#autotoc_md120", null ],
        [ "REQ5.3 Electric current is specified in amperes", "units.html#autotoc_md121", null ],
        [ "REQ5.4 Time is specified in seconds", "units.html#autotoc_md122", null ],
        [ "REQ5.5 Angular velocity is specified in radians per second.", "units.html#autotoc_md123", null ]
      ] ],
      [ "Configuration", "config.html", [
        [ "REQ6.1 One configuration", "config.html#autotoc_md124", null ],
        [ "REQ6.2 Configuration can be managed by the user via the communication interface", "config.html#autotoc_md125", [
          [ "REQ6.2.1 Get configuration field", "config.html#autotoc_md126", null ],
          [ "REQ6.2.2 Set configuration field", "config.html#autotoc_md127", null ],
          [ "REQ6.2.3 Commit the configuration", "config.html#autotoc_md128", null ],
          [ "REQ6.2.4 Clear the configuration", "config.html#autotoc_md129", null ]
        ] ],
        [ "REQ6.3 Configuration shall be documented", "config.html#autotoc_md130", null ],
        [ "REQ6.4 Configuration is tied to the version of firmware", "config.html#autotoc_md131", null ],
        [ "REQ6.5 Persistent configuration", "config.html#autotoc_md132", [
          [ "REQ6.5.1 Entire set is always stored", "config.html#autotoc_md133", null ],
          [ "REQ6.5.2 Size of history", "config.html#autotoc_md134", null ],
          [ "REQ6.5.3 Clear removes history", "config.html#autotoc_md135", null ],
          [ "REQ6.5.4 Firmware version is also stored", "config.html#autotoc_md136", null ]
        ] ],
        [ "REQ6.6 Firmware version changes", "config.html#autotoc_md137", null ],
        [ "REQ6.7 Utility for changes shall exist", "config.html#autotoc_md138", null ],
        [ "REQ6.8 Common format for storing on host side", "config.html#autotoc_md139", null ],
        [ "REQ6.9 List of fields for configuration", "config.html#autotoc_md140", [
          [ "REQ6.9.1 Model", "config.html#autotoc_md141", null ],
          [ "REQ6.9.2 ID", "config.html#autotoc_md142", null ]
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
"classservio_1_1drv_1_1dts__temp.html#a3833ead6722273c733f3adc7ff73e008",
"current_2cfg_8hpp.html",
"globals_8cpp.html",
"md_src_2iface_2iface.html",
"namespaceservio_1_1cfg.html#a215c26edc6be40e5ceb18f9e75f96e62",
"namespaceservio_1_1gov_1_1pos_1_1cfg.html#aaa160cd48f4ef9e5aab483168573d96d",
"namespaceservio_1_1plt.html#a9b84499420f471615a09ab94300d09e4",
"power_8cpp.html",
"str__lib_8hpp.html#aa45a43e85a14382a4f7b45d379c8c07e",
"structservio_1_1core_1_1core.html",
"structservio_1_1drv_1_1detailed__cb.html#a133cfe9835ab78ca47ed2fef1116a84c",
"structservio_1_1drv_1_1tests_1_1pwm__motor__test.html",
"structservio_1_1ftest_1_1bench_1_1loop__frequency.html",
"structservio_1_1gov_1_1engage__res.html#ae8298d91dc86efc9a6e1d40e04b38e77",
"structservio_1_1gov_1_1vel_1_1cfg_1_1map.html#a05aa280cb253417d0e4bc242941a604e",
"structservio_1_1linear__transition__regulator.html#a9e98e473898b4188b025c012fbc4f718",
"structservio_1_1scmdio_1_1serial__stream.html#aa6b00ecd161a0b8800178ee026be3eeb"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';