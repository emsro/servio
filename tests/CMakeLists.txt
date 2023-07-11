find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED)

function(add_fwlib_test)
  cmake_parse_arguments(A "" "TARGET" "ARGS" ${ARGN})
  add_executable(${A_TARGET} ${A_TARGET}.cpp)
  target_link_libraries(
    ${A_TARGET}
    GTest::GTest
    serviolib
    gtest_main
    nlohmann_json::nlohmann_json
    proto
    magic_enum)
  target_include_directories(${A_TARGET} PRIVATE ./include/)
  servio_compile_options(${A_TARGET})

  add_test(NAME ${A_TARGET}_test COMMAND ${A_TARGET})
endfunction()

add_fwlib_test(TARGET kalman_utest)
add_fwlib_test(TARGET control_utest)
add_fwlib_test(TARGET metrics_utest)
add_fwlib_test(TARGET cfg_storage_utest)
add_fwlib_test(TARGET cfg_utest)

function(add_btest)
  cmake_parse_arguments(A "" "TARGET" "ARGS;SOURCE" ${ARGN})
  add_executable(${A_TARGET} ${A_SOURCE})
  target_link_libraries(${A_TARGET} PRIVATE hostlib GTest::GTest)
  servio_compile_options(${A_TARGET})
endfunction()

add_btest(TARGET basic_btest SOURCE bb/basic.cpp)
add_btest(TARGET moves_btest SOURCE bb/moves.cpp)
