find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Boost 1.70 REQUIRED system program_options)


function(add_fwlib_test)
  cmake_parse_arguments(A "" "TARGET" "ARGS" ${ARGN})
  add_executable(${A_TARGET} ${A_TARGET}.cpp)
  target_link_libraries(${A_TARGET} GTest::GTest servio_lib gtest_main
                        nlohmann_json::nlohmann_json Boost::program_options)
  target_include_directories(${A_TARGET} PRIVATE ./include/)
  target_compile_options(${A_TARGET} PRIVATE -DEMLABCPP_USE_NLOHMANN_JSON -O0
                                             -g)
  servio_compile_options(${A_TARGET})

  add_test(NAME ${A_TARGET}_test COMMAND ${A_TARGET})
endfunction()

add_fwlib_test(TARGET kalman_utest)
add_fwlib_test(TARGET control_utest)
add_fwlib_test(TARGET metrics_utest)
