servio_add_library(
  TARGET fwlib
  SOURCES cfg_dispatcher.cpp
          dispatcher.cpp
          drv/cobs_uart.cpp
          drv/hbridge.cpp
          drv/leds.cpp
          extra.cpp
          store_persistent_config.cpp
          globals.cpp
          servio_pb.cpp
          util.cpp
  LIBS serviolib proto CMSIS::STM32::G431xx HAL::STM32::G4
  OPTS -Os)
