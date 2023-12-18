servio_add_library(
  TARGET fwlib
  SOURCES dispatcher.cpp
          drv/cobs_uart.cpp
          drv/leds.cpp
          drv/hbridge.cpp
          extra.cpp
          store_persistent_config.cpp
          servio_pb.cpp
          util.cpp
  LIBS serviolib proto platform
  OPTS -Os)
