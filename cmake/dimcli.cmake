cmake_policy(SET CMP0135 NEW)

include(FetchContent)

FetchContent_Declare(
  dimcli URL https://github.com/gknowles/dimcli/archive/refs/tags/v7.1.0.zip)
FetchContent_GetProperties(dimcli)
if(NOT dimcli_POPULATED)
  FetchContent_Populate(dimcli)
  add_library(dimcli STATIC ${dimcli_SOURCE_DIR}/libs/dimcli/cli.cpp)
  target_include_directories(dimcli SYSTEM PUBLIC ${dimcli_SOURCE_DIR}/libs)
endif()
