servio_add_library(
  TARGET prototype2_board
  INCLUDE ../
  SOURCES adc.cpp
          board.cpp
          gpio.cpp
          clock.cpp
          config.cpp
          uart.cpp
          timers.cpp
  LIBS emlabcpp platform
  OPTS -Os)
