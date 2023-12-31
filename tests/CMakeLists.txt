find_package(nlohmann_json REQUIRED)

function(add_btest)
  cmake_parse_arguments(A "" "TARGET" "ARGS;SOURCE" ${ARGN})
  add_executable(${A_TARGET} ${A_SOURCE})
  target_link_libraries(${A_TARGET} PRIVATE scmdiolib GTest::GTest)
  servio_compile_options(${A_TARGET})
endfunction()

add_btest(TARGET basic_btest SOURCE bb/basic.cpp)
add_btest(TARGET moves_btest SOURCE bb/moves.cpp)
