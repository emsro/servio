find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED)

function(add_utest target)
  servio_add_test(
    TARGET
    ${target}
    SOURCES
    ${target}.cpp
    INCLUDE
    ./include/
    LIBS
    GTest::GTest
    gtest_main
    serviolib
    nlohmann_json::nlohmann_json
    proto
    magic_enum)
endfunction()

add_utest(kalman_utest)
add_utest(control_utest)
add_utest(metrics_utest)
add_utest(cfg_storage_utest)
add_utest(cfg_utest)

function(add_btest)
  cmake_parse_arguments(A "" "TARGET" "ARGS;SOURCE" ${ARGN})
  add_executable(${A_TARGET} ${A_SOURCE})
  target_link_libraries(${A_TARGET} PRIVATE hostlib GTest::GTest)
  servio_compile_options(${A_TARGET})
endfunction()

add_btest(TARGET basic_btest SOURCE bb/basic.cpp)
add_btest(TARGET moves_btest SOURCE bb/moves.cpp)
