set(NANOPB_DIR ${CMAKE_SOURCE_DIR}/dep/nanopb/)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${NANOPB_DIR}/extra)

find_package(Nanopb REQUIRED)

add_library(
  nanopb_lib STATIC ${NANOPB_DIR}/pb_common.c ${NANOPB_DIR}/pb_encode.c
                    ${NANOPB_DIR}/pb_decode.c)
target_include_directories(nanopb_lib SYSTEM PUBLIC ${NANOPB_DIR})
