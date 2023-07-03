servio_add_library(
  TARGET fwlib
  SOURCES cfg_dispatcher.cpp
          dispatcher.cpp
          drivers/acquisition.cpp
          drivers/comms.cpp
          drivers/debug_comms.cpp
          drivers/hbridge.cpp
          drivers/leds.cpp
          extra.cpp
          globals.cpp
          servio_pb.cpp
          util.cpp
  LIBS serviolib proto CMSIS::STM32::G431xx HAL::STM32::G4
  OPTS -Os)
