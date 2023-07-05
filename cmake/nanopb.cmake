include(FetchContent)

FetchContent_Declare(
  nanopb
  GIT_REPOSITORY https://github.com/nanopb/nanopb.git
  GIT_TAG 0.4.7)
FetchContent_GetProperties(nanopb)
if(NOT nanopb_POPULATED)
  FetchContent_Populate(nanopb)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${nanopb_SOURCE_DIR}/extra)

find_package(Nanopb REQUIRED)

add_library(
  nanopb_lib STATIC
  ${nanopb_SOURCE_DIR}/pb_common.c ${nanopb_SOURCE_DIR}/pb_encode.c
  ${nanopb_SOURCE_DIR}/pb_decode.c)
target_include_directories(nanopb_lib PUBLIC ${NANOPB_INCLUDE_DIRS})
