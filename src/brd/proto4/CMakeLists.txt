servio_add_library(
  TARGET proto4_board
  SOURCES board.cpp
  LIBS serviolib platform
  OPTS -Os)

servio_add_board_executable(
  TARGET proto4_fw
  SOURCES ../../fw/main.cpp
  LIBS serviolib proto4_board
  OPTS -Os
  LINKER_SCRIPT
    "${CMAKE_CURRENT_SOURCE_DIR}/../../plt/stm32h5/STM32H503xx_FLASH.ld")

servio_add_board_executable(
  TARGET proto4_fw_ftest
  SOURCES tests.cpp
  LIBS serviolib ftestlib proto4_board
  OPTS -Os
  LINKER_SCRIPT
    "${CMAKE_CURRENT_SOURCE_DIR}/../../plt/stm32h5/STM32H503xx_FLASH.ld")

servio_add_board_executable(
  TARGET proto4_bench_ftest
  SOURCES bench.cpp
  LIBS serviolib ftestlib proto4_board
  OPTS -Os
  LINKER_SCRIPT
    "${CMAKE_CURRENT_SOURCE_DIR}/../../plt/stm32h5/STM32H503xx_FLASH.ld")
