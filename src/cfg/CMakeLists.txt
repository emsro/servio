function(servion_gen_cfg)
  cmake_parse_arguments(A "" "TARGET;DEF;OUTPUT;DOC" "" ${ARGN})
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${A_OUTPUT}
    COMMAND python3 ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gen.py --definition
            ${A_DEF} --output ${A_OUTPUT} --doc ${A_DOC}
    COMMENT "Generating configuration header and documentation from: ${A_DEF}"
    VERBATIM
    DEPENDS ${A_DEF} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gen.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_custom_target(${A_TARGET} DEPENDS ${A_OUTPUT} ${A_DOC})
endfunction()

servio_add_library(
  TARGET cfglib
  SOURCES dispatcher.cpp
  LIBS emlabcpp platform ifacelib avakar::atom vari zll
  OPTS -Os)
servion_gen_cfg(
  TARGET cfg_def
  DEF def.json
  OUTPUT def.hpp
  DOC cfg.md)
add_dependencies(cfglib cfg_def)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES
                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS cfglib)

endif()
