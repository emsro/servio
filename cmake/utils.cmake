function(servio_add_library)
  cmake_parse_arguments(A "" "TARGET" "INCLUDE;SOURCES;LIBS;OPTS" ${ARGN})

  add_library(${A_TARGET} OBJECT ${A_SOURCES})
  target_include_directories(${A_TARGET} PUBLIC ${A_INCLUDE})
  servio_compile_options(${A_TARGET})
  target_link_libraries(${A_TARGET} PUBLIC ${A_LIBS})
  target_compile_options(${A_TARGET} PRIVATE ${A_OPTS})

endfunction()

function(servio_add_board_executable)
  cmake_parse_arguments(A "" "TARGET;LINKER_SCRIPT" "INCLUDE;SOURCES;LIBS;OPTS"
                        ${ARGN})

  add_executable(${A_TARGET} ${A_SOURCES})
  target_include_directories(${A_TARGET} PUBLIC ${A_INCLUDE})
  servio_compile_options(${A_TARGET})
  target_link_libraries(${A_TARGET} PUBLIC ${A_LIBS})
  target_compile_options(${A_TARGET} PRIVATE ${A_OPTS})
  target_link_options(${A_TARGET} PRIVATE -Wl,--print-memory-usage)

  stm32_add_linker_script(${A_TARGET} PRIVATE ${A_LINKER_SCRIPT})
endfunction()

function(servio_add_test)
  cmake_parse_arguments(A "" "TARGET" "INCLUDE;SOURCES;LIBS;" ${ARGN})
  add_executable(${A_TARGET} ${A_SOURCES})
  target_include_directories(${A_TARGET} PUBLIC ${A_INCLUDE})
  servio_compile_options(${A_TARGET})
  target_link_libraries(${A_TARGET} ${A_LIBS})
  servio_compile_options(${A_TARGET})

  add_test(NAME ${A_TARGET}_test COMMAND ${A_TARGET})
endfunction()
