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
    [ "Architecture Decision Records", "index.html", null ],
    [ "1. Record architecture decisions", "md_doc_adr_0001_record_architecture_decisions.html", [
      [ "Status", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md1", null ],
      [ "Context", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md2", null ],
      [ "Decision", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md3", null ],
      [ "Consequences", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md4", null ]
    ] ],
    [ "2. Project shall be writen with C++20", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html", [
      [ "Status", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md6", null ],
      [ "Context", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md7", null ],
      [ "Decision", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md8", null ],
      [ "Consequences", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md9", null ]
    ] ],
    [ "3. STM32G4 is the first platform", "md_doc_adr_0003_stm32g4_is_the_first_platform.html", [
      [ "Status", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md11", null ],
      [ "Context", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md12", null ],
      [ "Decision", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md13", null ],
      [ "Consequences", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md14", null ]
    ] ],
    [ "4. Servo shall provide position control", "md_doc_adr_0004_servo_shall_provide_position_control.html", [
      [ "Status", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md16", null ],
      [ "Context", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md17", null ],
      [ "Decision", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md18", null ],
      [ "Consequences", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md19", null ]
    ] ],
    [ "5. Servo shall provide velocity control", "md_doc_adr_0005_servo_shall_provide_velocity_control.html", [
      [ "Status", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md21", null ],
      [ "Context", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md22", null ],
      [ "Decision", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md23", null ],
      [ "Consequences", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md24", null ]
    ] ],
    [ "6. Servo shall provide current control", "md_doc_adr_0006_servo_shall_provide_current_control.html", [
      [ "Status", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md26", null ],
      [ "Context", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md27", null ],
      [ "Decision", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md28", null ],
      [ "Consequences", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md29", null ]
    ] ],
    [ "7. Chained control loops architecture shall be used", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html", [
      [ "Status", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md31", null ],
      [ "Context", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md32", null ],
      [ "Decision", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md33", null ],
      [ "Consequences", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md34", null ]
    ] ],
    [ "8. CMake is the one only build system", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html", [
      [ "Status", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md36", null ],
      [ "Context", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md37", null ],
      [ "Decision", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md38", null ],
      [ "Consequences", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md39", null ]
    ] ],
    [ "9. If servo in position, velocity, or current mode, limits for all three modes shall be respected at once", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html", [
      [ "Status", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md41", null ],
      [ "Context", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md42", null ],
      [ "Decision", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md43", null ],
      [ "Consequences", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md44", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ],
        [ "Related Functions", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"acquisition_8cpp.html",
"classindication.html#aaeefb4a81531e0fd82015a1c4711acd6",
"md_doc_adr_0004_servo_shall_provide_position_control.html",
"structcfg_1_1payload.html#a3abc72d179aa82ee807ff02534a31dfb"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';