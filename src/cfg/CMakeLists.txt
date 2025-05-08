add_custom_command(
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/def.hpp
  COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/gen.py
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/def.json
          ${CMAKE_CURRENT_SOURCE_DIR}/gen.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(cfg_def DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/def.hpp)

servio_add_library(
  TARGET cfglib
  SOURCES dispatcher.cpp
  LIBS emlabcpp platform ctllib ifacelib avakar::atom vari
  OPTS -Os)
add_dependencies(cfglib cfg_def)

if(${SERVIO_PLATFORM} STREQUAL "host")

  servio_expand_test(TARGET servio_utest SOURCES
                     ${CMAKE_CURRENT_SOURCE_DIR}/tests/*utest.cpp LIBS cfglib)

endif()
