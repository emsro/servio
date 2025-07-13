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
    [ "Config fields", "md_src_2cfg_2cfg.html", null ],
    [ "Interface", "md_src_2iface_2iface.html", [
      [ "Types", "md_src_2iface_2iface.html#autotoc_md43", null ],
      [ "Commands", "md_src_2iface_2iface.html#autotoc_md50", [
        [ "group <tt>mode</tt>", "md_src_2iface_2iface.html#autotoc_md51", [
          [ "cmd <tt>mode disengaged</tt>", "md_src_2iface_2iface.html#autotoc_md52", null ],
          [ "cmd <tt>mode power <power = 0></tt>", "md_src_2iface_2iface.html#autotoc_md53", null ],
          [ "cmd <tt>mode current <current = 0.0></tt>", "md_src_2iface_2iface.html#autotoc_md54", null ],
          [ "cmd <tt>mode velocity <velocity = 0.0></tt>", "md_src_2iface_2iface.html#autotoc_md55", null ],
          [ "cmd <tt>mode position <position = 0.0></tt>", "md_src_2iface_2iface.html#autotoc_md56", null ]
        ] ],
        [ "cmd <tt>prop <name></tt>", "md_src_2iface_2iface.html#autotoc_md57", null ],
        [ "group <tt>cfg</tt>", "md_src_2iface_2iface.html#autotoc_md58", [
          [ "cmd <tt>cfg set <field> <value></tt>", "md_src_2iface_2iface.html#autotoc_md59", null ],
          [ "cmd <tt>cfg get <field></tt>", "md_src_2iface_2iface.html#autotoc_md60", null ],
          [ "cmd <tt>cfg list5 <level = \"\"> <offset = 0> <n = 5></tt>", "md_src_2iface_2iface.html#autotoc_md61", null ],
          [ "cmd <tt>cfg commit</tt>", "md_src_2iface_2iface.html#autotoc_md62", null ],
          [ "cmd <tt>cfg clear</tt>", "md_src_2iface_2iface.html#autotoc_md63", null ]
        ] ],
        [ "cmd <tt>info</tt>", "md_src_2iface_2iface.html#autotoc_md64", null ]
      ] ]
    ] ],
    [ "Motor related math", "md_doc_2math.html", [
      [ "Dictionary", "md_doc_2math.html#autotoc_md66", null ],
      [ "Equations", "md_doc_2math.html#autotoc_md67", [
        [ "velocity-torque curve", "md_doc_2math.html#autotoc_md68", null ]
      ] ]
    ] ],
    [ "Requirements", "md_doc_2requirements.html", [
      [ "System requirements", "sys.html", [
        [ "REQ1.1 Prioritize project independence to hardware", "sys.html#autotoc_md69", [
          [ "REQ1.1.1 Decomposition", "sys.html#autotoc_md70", null ],
          [ "REQ1.1.2 Code shall not be dependent on hardware specifics unless necessary", "sys.html#autotoc_md71", null ]
        ] ],
        [ "REQ1.2 Focus on testing", "sys.html#autotoc_md72", null ],
        [ "REQ1.2.1 Test on host what can be tested on host", "sys.html#autotoc_md73", null ],
        [ "REQ1.2.3 Use testing on target hardware", "sys.html#autotoc_md74", null ]
      ] ],
      [ "System control requirements", "ctl.html", [
        [ "REQ2.1 Position control mode", "ctl.html#autotoc_md75", null ],
        [ "REQ2.2 Current control mode", "ctl.html#autotoc_md76", null ],
        [ "REQ2.3 Velocity control mode", "ctl.html#autotoc_md77", null ],
        [ "REQ2.4 Power control mode", "ctl.html#autotoc_md78", null ],
        [ "REQ2.5 Movement detection", "ctl.html#autotoc_md79", null ],
        [ "REQ2.6 Static friction compensation", "ctl.html#autotoc_md80", null ],
        [ "REQ2.7 Disengaged", "ctl.html#autotoc_md81", null ]
      ] ],
      [ "System limits requirements", "lims.html", [
        [ "REQ3.1 Current limits", "lims.html#autotoc_md82", null ],
        [ "REQ3.2 Position limits", "lims.html#autotoc_md83", null ],
        [ "REQ3.3 Velocity limits", "lims.html#autotoc_md84", null ]
      ] ],
      [ "Communication requirements", "comms.html", [
        [ "REQ4.1 System should have one major communication interface", "comms.html#autotoc_md85", null ],
        [ "REQ4.2 System should decompose message communication to be independent on underlying transport layer", "comms.html#autotoc_md86", null ],
        [ "RE4.3 UART shall be supported as transport", "comms.html#autotoc_md87", null ],
        [ "REQ4.4 Major communication message shall be defined in TBD", "comms.html#autotoc_md88", null ],
        [ "REQ4.5 COBS can be used as framing mechanism", "comms.html#autotoc_md89", null ],
        [ "REQ4.6 Exact communication interface is property of board", "comms.html#autotoc_md90", null ],
        [ "REQ4.7 ID", "comms.html#autotoc_md91", null ],
        [ "REQ4.8 Group ID", "comms.html#autotoc_md92", null ]
      ] ],
      [ "Units", "units.html", [
        [ "REQ5.1 Any values facing the user shall be in well defined units specified by other requirements", "units.html#autotoc_md93", null ],
        [ "REQ5.2 Angular position is specified in radians", "units.html#autotoc_md94", null ],
        [ "REQ5.3 Electric current is specified in amperes", "units.html#autotoc_md95", null ],
        [ "REQ5.4 Time is specified in seconds", "units.html#autotoc_md96", null ],
        [ "REQ5.5 Angular velocity is specified in radians per second.", "units.html#autotoc_md97", null ]
      ] ],
      [ "Configuration", "config.html", [
        [ "REQ6.1 One configuration", "config.html#autotoc_md98", null ],
        [ "REQ6.2 Configuration can be managed by the user via the communication interface", "config.html#autotoc_md99", [
          [ "REQ6.2.1 Get configuration field", "config.html#autotoc_md100", null ],
          [ "REQ6.2.2 Set configuration field", "config.html#autotoc_md101", null ],
          [ "REQ6.2.3 Commit the configuration", "config.html#autotoc_md102", null ],
          [ "REQ6.2.4 Clear the configuration", "config.html#autotoc_md103", null ]
        ] ],
        [ "REQ6.3 Configuration shall be documented", "config.html#autotoc_md104", null ],
        [ "REQ6.4 Configuration is tied to the version of firmware", "config.html#autotoc_md105", null ],
        [ "REQ6.5 Persistent configuration", "config.html#autotoc_md106", [
          [ "REQ6.5.1 Entire set is always stored", "config.html#autotoc_md107", null ],
          [ "REQ6.5.2 Size of history", "config.html#autotoc_md108", null ],
          [ "REQ6.5.3 Clear removes history", "config.html#autotoc_md109", null ],
          [ "REQ6.5.4 Firmware version is also stored", "config.html#autotoc_md110", null ]
        ] ],
        [ "REQ6.6 Firmware version changes", "config.html#autotoc_md111", null ],
        [ "REQ6.7 Utility for changes shall exist", "config.html#autotoc_md112", null ],
        [ "REQ6.8 Common format for storing on host side", "config.html#autotoc_md113", null ],
        [ "REQ6.9 List of fields for configuration", "config.html#autotoc_md114", [
          [ "REQ6.9.1 Model", "config.html#autotoc_md115", null ],
          [ "REQ6.9.2 ID", "config.html#autotoc_md116", null ]
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
"char__uart_8cpp.html",
"cobs__rx__container_8hpp.html",
"functions_func_n.html",
"md_src_2iface_2iface.html#autotoc_md55",
"namespaceservio_1_1cfg.html#a17a7e0a2edf8384906b0f662d462cfd3a3879788c8c54de8f9cc05a7ffa5cecdd",
"namespaceservio_1_1ftester.html#a4e2bf400b5caac347ef36aaa7b2d04ef",
"namespaceservio_1_1scmdio.html#a77ea38e1834f94b476247a7987fd8a15",
"scmdio_2main_8cpp.html#aa895c4d04b8c0c7d854041c055f68740",
"structservio_1_1atom__sv__key.html",
"structservio_1_1drv_1_1adc__channel.html#a31f781d219a2cc4489adcbaba591da95",
"structservio_1_1drv_1_1leds__iface.html#ac254678ba7f928f8dba50189763f5074",
"structservio_1_1ftest_1_1bench_1_1loop__frequency.html#a8d8328a7e932e093b8b3eacf7e93fff6",
"structservio_1_1iface_1_1cfg__list5__stmt.html#a8ebfd2d070aa1cbf31d23f291aada0ce",
"structservio_1_1parser_1_1parser.html#a591bd45983f6e9eeb90b94a20c793874",
"structservio_1_1sntr_1_1test__central__sentry.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';