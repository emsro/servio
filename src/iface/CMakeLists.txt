function(servio_gen_iface)
  cmake_parse_arguments(A "" "TARGET;DEF;HDR;PARSER;DOC" "" ${ARGN})
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${A_HDR}
    COMMAND python3 ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gen.py --definition
            ${A_DEF} --hdr ${A_HDR} --parser ${A_PARSER} --doc ${A_DOC}
    COMMENT
      "Generating interface header, parser, and documentation from: def.json"
    VERBATIM
    DEPENDS ${A_DEF} ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/gen.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  add_custom_target(${A_TARGET} DEPENDS ${A_HDR} ${A_DOC})
endfunction()

servio_gen_iface(
  TARGET ifacelib_def
  DEF def.json
  HDR def.hpp
  PARSER parse.cpp
  DOC iface.md)
servio_add_library(
  TARGET ifacelib
  SOURCES parse.cpp
  LIBS avakar::atom vari emlabcpp platform
  OPTS -Og)
add_dependencies(ifacelib ifacelib_def)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES
                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS ifacelib)

endif()
