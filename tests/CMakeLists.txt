find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED)

function(add_fwlib_test)
  cmake_parse_arguments(A "" "TARGET" "ARGS" ${ARGN})
  add_executable(${A_TARGET} ${A_TARGET}.cpp)
  target_link_libraries(${A_TARGET} GTest::GTest serviolib gtest_main
                        nlohmann_json::nlohmann_json)
  target_include_directories(${A_TARGET} PRIVATE ./include/)
  servio_compile_options(${A_TARGET})

  add_test(NAME ${A_TARGET}_test COMMAND ${A_TARGET})
endfunction()

add_fwlib_test(TARGET kalman_utest)
add_fwlib_test(TARGET control_utest)
add_fwlib_test(TARGET metrics_utest)
add_fwlib_test(TARGET cfg_storage_utest)

add_executable(bb_basic bb/basic.cpp)
target_link_libraries(bb_basic PRIVATE hostlib args GTest::GTest)
servio_compile_options(bb_basic)
