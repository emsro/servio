macro(servio_add_library)
  cmake_parse_arguments(A "" "TARGET" "INCLUDE;SOURCES;LIBS;OPTS" ${ARGN})

  add_library(${A_TARGET} STATIC ${A_SOURCES})
  target_include_directories(${A_TARGET} PUBLIC ${A_INCLUDE})
  servio_compile_options(${A_TARGET})
  target_link_libraries(${A_TARGET} PUBLIC ${A_LIBS})
  target_compile_options(${A_TARGET} PRIVATE ${A_OPTS})
endmacro()

macro(servio_add_executable)
  cmake_parse_arguments(A "" "TARGET" "INCLUDE;SOURCES;LIBS;OPTS" ${ARGN})

  add_executable(${A_TARGET} ${A_SOURCES})
  target_include_directories(${A_TARGET} PUBLIC ${A_INCLUDE})
  servio_compile_options(${A_TARGET})
  target_link_libraries(${A_TARGET} PUBLIC ${A_LIBS})
  target_compile_options(${A_TARGET} PRIVATE ${A_OPTS})
endmacro()

macro(servio_add_board_executable)
  cmake_parse_arguments(A "" "TARGET;LINKER_SCRIPT" "INCLUDE;SOURCES;LIBS;OPTS"
                        ${ARGN})

  add_executable(${A_TARGET} ${A_SOURCES})
  target_include_directories(${A_TARGET} PUBLIC ${A_INCLUDE})
  servio_compile_options(${A_TARGET})
  target_link_libraries(${A_TARGET} PUBLIC ${A_LIBS})
  target_compile_options(${A_TARGET} PRIVATE ${A_OPTS})
  target_link_options(${A_TARGET} PRIVATE -Wl,--print-memory-usage)

  stm32_add_linker_script(${A_TARGET} PRIVATE ${A_LINKER_SCRIPT})
  stm32_generate_binary_file(${A_TARGET})
  stm32_generate_hex_file(${A_TARGET})
endmacro()

macro(servio_expand_test)
  cmake_parse_arguments(A "" "TARGET" "SOURCES;LIBS;" ${ARGN})
  file(GLOB source ${A_SOURCES})
  target_sources(${A_TARGET} PRIVATE ${source})
  target_link_libraries(${A_TARGET} PRIVATE ${A_LIBS})
endmacro()