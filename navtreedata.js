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
    [ "Requirements", "md_doc_2requirements.html", [
      [ "System requirements", "sys.html", [
        [ "REQ1.1 Prioritize project independence to hardware", "sys.html#autotoc_md6", [
          [ "REQ1.1.1 Decomposition", "sys.html#autotoc_md7", null ],
          [ "REQ1.1.2 Code shall not be dependent on hardware specifics unless necessary", "sys.html#autotoc_md8", null ]
        ] ],
        [ "REQ1.2 Focus on testing", "sys.html#autotoc_md9", null ],
        [ "REQ1.2.1 Test on host what can be tested on host", "sys.html#autotoc_md10", null ],
        [ "REQ1.2.3 Use testing on target hardware", "sys.html#autotoc_md11", null ]
      ] ],
      [ "System control requirements", "ctl.html", [
        [ "REQ2.1 Position control mode", "ctl.html#autotoc_md12", null ],
        [ "REQ2.2 Current control mode", "ctl.html#autotoc_md13", null ],
        [ "REQ2.3 Velocity control mode", "ctl.html#autotoc_md14", null ],
        [ "REQ2.4 Power control mode", "ctl.html#autotoc_md15", null ],
        [ "REQ2.5 Movement detection", "ctl.html#autotoc_md16", null ],
        [ "REQ2.6 Static friction compensation", "ctl.html#autotoc_md17", null ],
        [ "REQ2.7 Disengaged", "ctl.html#autotoc_md18", null ]
      ] ],
      [ "System limits requirements", "lims.html", [
        [ "REQ3.1 Current limits", "lims.html#autotoc_md19", null ],
        [ "REQ3.2 Position limits", "lims.html#autotoc_md20", null ],
        [ "REQ3.3 Velocity limits", "lims.html#autotoc_md21", null ]
      ] ],
      [ "Communication requirements", "comms.html", [
        [ "REQ4.1 System should have one major communication interface", "comms.html#autotoc_md22", null ],
        [ "REQ4.2 System should decompose message communication to be independent on underlying transport layer", "comms.html#autotoc_md23", null ],
        [ "RE4.3 UART shall be supported as transport", "comms.html#autotoc_md24", null ],
        [ "REQ4.4 Major communication message shall be defined in TBD", "comms.html#autotoc_md25", null ],
        [ "REQ4.5 COBS can be used as framing mechanism", "comms.html#autotoc_md26", null ],
        [ "REQ4.6 Exact communication interface is property of board", "comms.html#autotoc_md27", null ],
        [ "REQ4.7 ID", "comms.html#autotoc_md28", null ],
        [ "REQ4.8 Group ID", "comms.html#autotoc_md29", null ]
      ] ],
      [ "Units", "units.html", [
        [ "REQ5.1 Any values facing the user shall be in well defined units specified by other requirements", "units.html#autotoc_md30", null ],
        [ "REQ5.2 Angular position is specified in radians", "units.html#autotoc_md31", null ],
        [ "REQ5.3 Electric current is specified in amperes", "units.html#autotoc_md32", null ],
        [ "REQ5.4 Time is specified in seconds", "units.html#autotoc_md33", null ],
        [ "REQ5.5 Angular velocity is specified in radians per second.", "units.html#autotoc_md34", null ]
      ] ],
      [ "Configuration", "config.html", [
        [ "REQ6.1 One configuration", "config.html#autotoc_md35", null ],
        [ "REQ6.2 Configuration can be managed by the user via the communication interface", "config.html#autotoc_md36", [
          [ "REQ6.2.1 Get configuration field", "config.html#autotoc_md37", null ],
          [ "REQ6.2.2 Set configuration field", "config.html#autotoc_md38", null ],
          [ "REQ6.2.3 Commit the configuration", "config.html#autotoc_md39", null ],
          [ "REQ6.2.4 Clear the configuration", "config.html#autotoc_md40", null ]
        ] ],
        [ "REQ6.3 Configuration shall be documented", "config.html#autotoc_md41", null ],
        [ "REQ6.4 Configuration is tied to the version of firmware", "config.html#autotoc_md42", null ],
        [ "REQ6.5 Persistent configuration", "config.html#autotoc_md43", [
          [ "REQ6.5.1 Entire set is always stored", "config.html#autotoc_md44", null ],
          [ "REQ6.5.2 Size of history", "config.html#autotoc_md45", null ],
          [ "REQ6.5.3 Clear removes history", "config.html#autotoc_md46", null ],
          [ "REQ6.5.4 Firmware version is also stored", "config.html#autotoc_md47", null ]
        ] ],
        [ "REQ6.6 Firmware version changes", "config.html#autotoc_md48", null ],
        [ "REQ6.7 Utility for changes shall exist", "config.html#autotoc_md49", null ],
        [ "REQ6.8 Common format for storing on host side", "config.html#autotoc_md50", null ],
        [ "REQ6.9 List of fields for configuration", "config.html#autotoc_md51", [
          [ "REQ6.9.1 Model", "config.html#autotoc_md52", null ],
          [ "REQ6.9.2 ID", "config.html#autotoc_md53", null ]
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
        [ "Typedefs", "globals_type.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"adc_8cpp.html",
"classservio_1_1drv_1_1hbridge.html#ac6ff7accf723c388d176c3295598233c",
"def_8h.html#a07f5b32871871c0c582ce5bae32d01aa",
"hierarchy.html",
"namespaceservio.html#a546e51ff861bb4e2219e0b99efd5184f",
"namespaceservio_1_1drv.html#a939143e8846f4165956444ad9cdc5c4d",
"namespaceservio_1_1plt.html#ab37835d18590cef3c440c59263d761d2",
"proto3_2board_8cpp.html#a491410644cceae6a73091be92aa6f5d5",
"stm32h5xx__hal__conf_8h.html#a5c7c04d57c22f5301f1ea589abc6f35f",
"structservio_1_1core_1_1core.html#af79a219de96e8d0dfcf16ed14b8f8d0b",
"structservio_1_1drv_1_1detailed__cb.html",
"structservio_1_1drv_1_1tests_1_1temperature__test.html",
"structservio_1_1ftester_1_1test__system.html#af0406a1f2b2ffa877583fc3314dc7238",
"structservio_1_1scmdio_1_1kval__ser_3_01vari_1_1typelist_3_01KV_00_01KVs_8_8_8_01_4_01_4.html#a09f6da468761a0df3b2b8fc366f07664",
"structstd_1_1formatter_3_01nlohmann_1_1json_00_01char_01_4.html#aa16b5ede6779436ada33d5e6a1090aff"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';