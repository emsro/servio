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
    [ "Motor related math", "md_doc_2math.html", [
      [ "Dictionary", "md_doc_2math.html#autotoc_md7", null ],
      [ "Equations", "md_doc_2math.html#autotoc_md8", [
        [ "velocity-torque curve", "md_doc_2math.html#autotoc_md9", null ]
      ] ]
    ] ],
    [ "Requirements", "md_doc_2requirements.html", [
      [ "System requirements", "sys.html", [
        [ "REQ1.1 Prioritize project independence to hardware", "sys.html#autotoc_md10", [
          [ "REQ1.1.1 Decomposition", "sys.html#autotoc_md11", null ],
          [ "REQ1.1.2 Code shall not be dependent on hardware specifics unless necessary", "sys.html#autotoc_md12", null ]
        ] ],
        [ "REQ1.2 Focus on testing", "sys.html#autotoc_md13", null ],
        [ "REQ1.2.1 Test on host what can be tested on host", "sys.html#autotoc_md14", null ],
        [ "REQ1.2.3 Use testing on target hardware", "sys.html#autotoc_md15", null ]
      ] ],
      [ "System control requirements", "ctl.html", [
        [ "REQ2.1 Position control mode", "ctl.html#autotoc_md16", null ],
        [ "REQ2.2 Current control mode", "ctl.html#autotoc_md17", null ],
        [ "REQ2.3 Velocity control mode", "ctl.html#autotoc_md18", null ],
        [ "REQ2.4 Power control mode", "ctl.html#autotoc_md19", null ],
        [ "REQ2.5 Movement detection", "ctl.html#autotoc_md20", null ],
        [ "REQ2.6 Static friction compensation", "ctl.html#autotoc_md21", null ],
        [ "REQ2.7 Disengaged", "ctl.html#autotoc_md22", null ]
      ] ],
      [ "System limits requirements", "lims.html", [
        [ "REQ3.1 Current limits", "lims.html#autotoc_md23", null ],
        [ "REQ3.2 Position limits", "lims.html#autotoc_md24", null ],
        [ "REQ3.3 Velocity limits", "lims.html#autotoc_md25", null ]
      ] ],
      [ "Communication requirements", "comms.html", [
        [ "REQ4.1 System should have one major communication interface", "comms.html#autotoc_md26", null ],
        [ "REQ4.2 System should decompose message communication to be independent on underlying transport layer", "comms.html#autotoc_md27", null ],
        [ "RE4.3 UART shall be supported as transport", "comms.html#autotoc_md28", null ],
        [ "REQ4.4 Major communication message shall be defined in TBD", "comms.html#autotoc_md29", null ],
        [ "REQ4.5 COBS can be used as framing mechanism", "comms.html#autotoc_md30", null ],
        [ "REQ4.6 Exact communication interface is property of board", "comms.html#autotoc_md31", null ],
        [ "REQ4.7 ID", "comms.html#autotoc_md32", null ],
        [ "REQ4.8 Group ID", "comms.html#autotoc_md33", null ]
      ] ],
      [ "Units", "units.html", [
        [ "REQ5.1 Any values facing the user shall be in well defined units specified by other requirements", "units.html#autotoc_md34", null ],
        [ "REQ5.2 Angular position is specified in radians", "units.html#autotoc_md35", null ],
        [ "REQ5.3 Electric current is specified in amperes", "units.html#autotoc_md36", null ],
        [ "REQ5.4 Time is specified in seconds", "units.html#autotoc_md37", null ],
        [ "REQ5.5 Angular velocity is specified in radians per second.", "units.html#autotoc_md38", null ]
      ] ],
      [ "Configuration", "config.html", [
        [ "REQ6.1 One configuration", "config.html#autotoc_md39", null ],
        [ "REQ6.2 Configuration can be managed by the user via the communication interface", "config.html#autotoc_md40", [
          [ "REQ6.2.1 Get configuration field", "config.html#autotoc_md41", null ],
          [ "REQ6.2.2 Set configuration field", "config.html#autotoc_md42", null ],
          [ "REQ6.2.3 Commit the configuration", "config.html#autotoc_md43", null ],
          [ "REQ6.2.4 Clear the configuration", "config.html#autotoc_md44", null ]
        ] ],
        [ "REQ6.3 Configuration shall be documented", "config.html#autotoc_md45", null ],
        [ "REQ6.4 Configuration is tied to the version of firmware", "config.html#autotoc_md46", null ],
        [ "REQ6.5 Persistent configuration", "config.html#autotoc_md47", [
          [ "REQ6.5.1 Entire set is always stored", "config.html#autotoc_md48", null ],
          [ "REQ6.5.2 Size of history", "config.html#autotoc_md49", null ],
          [ "REQ6.5.3 Clear removes history", "config.html#autotoc_md50", null ],
          [ "REQ6.5.4 Firmware version is also stored", "config.html#autotoc_md51", null ]
        ] ],
        [ "REQ6.6 Firmware version changes", "config.html#autotoc_md52", null ],
        [ "REQ6.7 Utility for changes shall exist", "config.html#autotoc_md53", null ],
        [ "REQ6.8 Common format for storing on host side", "config.html#autotoc_md54", null ],
        [ "REQ6.9 List of fields for configuration", "config.html#autotoc_md55", [
          [ "REQ6.9.1 Model", "config.html#autotoc_md56", null ],
          [ "REQ6.9.2 ID", "config.html#autotoc_md57", null ]
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
        [ "Enumerator", "functions_eval.html", null ]
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
"classservio_1_1core_1_1current__callback.html#a961cdfdf8e89b6360bbb95e537e61e10",
"cobs__utest_8cpp.html#aee346fa297157efeaa914ac49cbd5206",
"functions_g.html",
"moves__test_8cpp.html#a419ee5b44dab39f4b23a8503efa74383",
"namespaceservio_1_1core.html#a22ae2baca758d55f044e3becfaab35d3",
"namespaceservio_1_1parser.html#structservio_1_1parser_1_1id",
"parse__utest_8cpp.html#aa6ca13fc37760e880ae7679695c73de7",
"stm32h5xx__hal__conf_8h.html#a76205fe6419c395659ba3c31954bc2a9",
"structservio_1_1cfg_1_1map.html#ae7568859cf248409ec53388d2f2370a6",
"structservio_1_1drv_1_1cobs__uart.html#a5574a0c2bfe74c1e6b50c47b49abb68c",
"structservio_1_1drv_1_1tests_1_1cobs__uart__err__test.html#a44ac8cc257b5a359e6e2ad9cdda17723",
"structservio_1_1ftest_1_1testing__system.html#a14956559e6460df15eee9dd9ab531854",
"structservio_1_1mon_1_1blinker.html#aafb4c78f56d665b361b2335eb29fff34",
"structservio_1_1scmdio_1_1serial__stream.html#a163fbc51b008b677dc60181a79b81029",
"uart_8hpp.html#a6b247372630cfbf78e3f602e71389a49"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';