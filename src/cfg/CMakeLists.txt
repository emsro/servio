function(servio_add_configuration)
  cmake_parse_arguments(A "" "TARGET;DEF;OUTPUT;DOC" "" ${ARGN})
  add_custom_command(
    OUTPUT ${A_OUTPUT}
    COMMAND python3 ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gen.py --definition
            ${A_DEF} --output ${A_OUTPUT} --doc ${A_DOC}
    COMMENT "Generating configuration header and documentation from: ${A_DEF}"
    VERBATIM
    DEPENDS ${A_DEF} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gen.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_custom_target(${A_TARGET} DEPENDS ${A_OUTPUT})
endfunction()

servio_add_library(
  TARGET cfglib
  SOURCES dispatcher.cpp
  LIBS emlabcpp platform ctllib ifacelib avakar::atom vari
  OPTS -Os)
servio_add_configuration(
  TARGET
  cfg_def
  DEF
  def.json
  OUTPUT
  def.hpp
  DOC
  cfg.md)
add_dependencies(cfglib cfg_def)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES
                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS cfglib)

endif()
