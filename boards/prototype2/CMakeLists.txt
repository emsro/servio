servio_add_library(
  TARGET prototype2_board
  SOURCES adc.cpp
          board.cpp
          gpio.cpp
          clock.cpp
          config.cpp
          uart.cpp
          timers.cpp
  LIBS serviolib
  OPTS -Os)
