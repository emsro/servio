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
    [ "Install", "index.html#autotoc_md1", null ],
    [ "Build", "index.html#autotoc_md2", null ],
    [ "1. Record architecture decisions", "md_doc_adr_0001_record_architecture_decisions.html", [
      [ "Status", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md4", null ],
      [ "Context", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md5", null ],
      [ "Decision", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md6", null ],
      [ "Consequences", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md7", null ]
    ] ],
    [ "2. Project shall be writen with C++20", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html", [
      [ "Status", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md9", null ],
      [ "Context", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md10", null ],
      [ "Decision", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md11", null ],
      [ "Consequences", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md12", null ]
    ] ],
    [ "3. STM32G4 is the first platform", "md_doc_adr_0003_stm32g4_is_the_first_platform.html", [
      [ "Status", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md14", null ],
      [ "Context", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md15", null ],
      [ "Decision", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md16", null ],
      [ "Consequences", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md17", null ]
    ] ],
    [ "4. Servo shall provide position control", "md_doc_adr_0004_servo_shall_provide_position_control.html", [
      [ "Status", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md19", null ],
      [ "Context", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md20", null ],
      [ "Decision", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md21", null ],
      [ "Consequences", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md22", null ]
    ] ],
    [ "5. Servo shall provide velocity control", "md_doc_adr_0005_servo_shall_provide_velocity_control.html", [
      [ "Status", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md24", null ],
      [ "Context", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md25", null ],
      [ "Decision", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md26", null ],
      [ "Consequences", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md27", null ]
    ] ],
    [ "6. Servo shall provide current control", "md_doc_adr_0006_servo_shall_provide_current_control.html", [
      [ "Status", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md29", null ],
      [ "Context", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md30", null ],
      [ "Decision", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md31", null ],
      [ "Consequences", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md32", null ]
    ] ],
    [ "7. Chained control loops architecture shall be used", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html", [
      [ "Status", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md34", null ],
      [ "Context", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md35", null ],
      [ "Decision", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md36", null ],
      [ "Consequences", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md37", null ]
    ] ],
    [ "8. CMake is the one only build system", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html", [
      [ "Status", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md39", null ],
      [ "Context", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md40", null ],
      [ "Decision", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md41", null ],
      [ "Consequences", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md42", null ]
    ] ],
    [ "9. If servo in position, velocity, or current mode, limits for all three modes shall be respected at once", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html", [
      [ "Status", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md44", null ],
      [ "Context", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md45", null ],
      [ "Decision", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md46", null ],
      [ "Consequences", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md47", null ]
    ] ],
    [ "10. STM32H5 is the main platform", "md_doc_adr_0010_stm32h5_is_the_main_platform.html", [
      [ "Status", "md_doc_adr_0010_stm32h5_is_the_main_platform.html#autotoc_md49", null ],
      [ "Context", "md_doc_adr_0010_stm32h5_is_the_main_platform.html#autotoc_md50", null ],
      [ "Decision", "md_doc_adr_0010_stm32h5_is_the_main_platform.html#autotoc_md51", null ],
      [ "Consequences", "md_doc_adr_0010_stm32h5_is_the_main_platform.html#autotoc_md52", null ]
    ] ],
    [ "11. Modular control loop architecture shall be used", "md_doc_adr_0011_modular_control_loop_architecture_shall_be_used.html", [
      [ "Status", "md_doc_adr_0011_modular_control_loop_architecture_shall_be_used.html#autotoc_md54", null ],
      [ "Context", "md_doc_adr_0011_modular_control_loop_architecture_shall_be_used.html#autotoc_md55", null ],
      [ "Decision", "md_doc_adr_0011_modular_control_loop_architecture_shall_be_used.html#autotoc_md56", null ],
      [ "Consequences", "md_doc_adr_0011_modular_control_loop_architecture_shall_be_used.html#autotoc_md57", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", "namespacemembers_dup" ],
        [ "Functions", "namespacemembers_func.html", null ],
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
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Related Functions", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", null ],
        [ "Variables", "globals_vars.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"adc__pooler_8hpp.html",
"cli_8cpp.html#a0f5027194b08215b948567a9932bcdbc",
"key_8hpp.html#a17a7e0a2edf8384906b0f662d462cfd3a62ab6a965f4c3bf5ccc7865227d7aada",
"namespaceservio_1_1brd.html#structservio_1_1brd_1_1setpoint",
"namespaceservio_1_1scmdio.html#a0fc0b5d25c4f3c4f607000350a9d4105",
"stm32g4xx__hal__conf_8h.html#a52b8d9123261d716ad98a5c3db52c2ed",
"structservio_1_1ctl_1_1linear__transition__regulator.html#aecb19b527ceb961ed007cbbd7ed5aca0"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';