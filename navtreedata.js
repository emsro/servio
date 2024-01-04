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
    [ "1. Record architecture decisions", "md_doc_adr_0001_record_architecture_decisions.html", [
      [ "Status", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md7", null ],
      [ "Context", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md8", null ],
      [ "Decision", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md9", null ],
      [ "Consequences", "md_doc_adr_0001_record_architecture_decisions.html#autotoc_md10", null ]
    ] ],
    [ "2. Project shall be writen with C++20", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html", [
      [ "Status", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md12", null ],
      [ "Context", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md13", null ],
      [ "Decision", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md14", null ],
      [ "Consequences", "md_doc_adr_0002_project_shall_be_writen_with_c_20.html#autotoc_md15", null ]
    ] ],
    [ "3. STM32G4 is the first platform", "md_doc_adr_0003_stm32g4_is_the_first_platform.html", [
      [ "Status", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md17", null ],
      [ "Context", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md18", null ],
      [ "Decision", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md19", null ],
      [ "Consequences", "md_doc_adr_0003_stm32g4_is_the_first_platform.html#autotoc_md20", null ]
    ] ],
    [ "4. Servo shall provide position control", "md_doc_adr_0004_servo_shall_provide_position_control.html", [
      [ "Status", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md22", null ],
      [ "Context", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md23", null ],
      [ "Decision", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md24", null ],
      [ "Consequences", "md_doc_adr_0004_servo_shall_provide_position_control.html#autotoc_md25", null ]
    ] ],
    [ "5. Servo shall provide velocity control", "md_doc_adr_0005_servo_shall_provide_velocity_control.html", [
      [ "Status", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md27", null ],
      [ "Context", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md28", null ],
      [ "Decision", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md29", null ],
      [ "Consequences", "md_doc_adr_0005_servo_shall_provide_velocity_control.html#autotoc_md30", null ]
    ] ],
    [ "6. Servo shall provide current control", "md_doc_adr_0006_servo_shall_provide_current_control.html", [
      [ "Status", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md32", null ],
      [ "Context", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md33", null ],
      [ "Decision", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md34", null ],
      [ "Consequences", "md_doc_adr_0006_servo_shall_provide_current_control.html#autotoc_md35", null ]
    ] ],
    [ "7. Chained control loops architecture shall be used", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html", [
      [ "Status", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md37", null ],
      [ "Context", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md38", null ],
      [ "Decision", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md39", null ],
      [ "Consequences", "md_doc_adr_0007_chained_control_loops_architecture_shall_be_used.html#autotoc_md40", null ]
    ] ],
    [ "8. CMake is the one only build system", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html", [
      [ "Status", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md42", null ],
      [ "Context", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md43", null ],
      [ "Decision", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md44", null ],
      [ "Consequences", "md_doc_adr_0008_cmake_is_the_one_only_build_system.html#autotoc_md45", null ]
    ] ],
    [ "9. If servo in position, velocity, or current mode, limits for all three modes shall be respected at once", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html", [
      [ "Status", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md47", null ],
      [ "Context", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md48", null ],
      [ "Decision", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md49", null ],
      [ "Consequences", "md_doc_adr_0009_if_servo_in_position_velocity_or_current_mode_limits_for_all_three_modes_shall_be_respected_at_once.html#autotoc_md50", null ]
    ] ],
    [ "10. STM32H5 is the main platform", "md_doc_adr_0010_stm32h5_is_the_main_platform.html", [
      [ "Status", "md_doc_adr_0010_stm32h5_is_the_main_platform.html#autotoc_md52", null ],
      [ "Context", "md_doc_adr_0010_stm32h5_is_the_main_platform.html#autotoc_md53", null ],
      [ "Decision", "md_doc_adr_0010_stm32h5_is_the_main_platform.html#autotoc_md54", null ],
      [ "Consequences", "md_doc_adr_0010_stm32h5_is_the_main_platform.html#autotoc_md55", null ]
    ] ],
    [ "11. Modular control loop architecture shall be used", "md_doc_adr_0011_modular_control_loop_architecture_shall_be_used.html", [
      [ "Status", "md_doc_adr_0011_modular_control_loop_architecture_shall_be_used.html#autotoc_md57", null ],
      [ "Context", "md_doc_adr_0011_modular_control_loop_architecture_shall_be_used.html#autotoc_md58", null ],
      [ "Decision", "md_doc_adr_0011_modular_control_loop_architecture_shall_be_used.html#autotoc_md59", null ],
      [ "Consequences", "md_doc_adr_0011_modular_control_loop_architecture_shall_be_used.html#autotoc_md60", null ]
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
        [ "Typedefs", "globals_type.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"",
"classservio_1_1mon_1_1indication.html#a26ecc3736d0ec32e0a42f254baa5e739",
"kalman_8hpp.html#aab9b4531c387d4264fe17a1341856597",
"namespaceservio_1_1brd.html#a5bce2edc1a38d3055b24c79c9b7a24fc",
"namespaceservio_1_1plt.html#a03cac8a5dbc15f966a3b411492fffb0d",
"servio__pb_8hpp.html#a68b10983c43a8988f0421b90a7ad8adf",
"structservio_1_1bb_1_1bb__test__case.html#a0ca98012019603e8f74a014e6220c107"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';