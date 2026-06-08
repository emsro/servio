file(GLOB HAL_SOURCES ./Src/*.c)

servio_add_library(
  TARGET stm32h5_hal
  SOURCES ${HAL_SOURCES} ./cmsis/system_stm32h5xx.c ./startup_stm32h503xx.s
  OPTS -Os -w)
target_compile_options(
  stm32h5_hal
  PUBLIC -mcpu=cortex-m33
         -mfloat-abi=hard
         -mfpu=fpv5-sp-d16
         -mthumb
         -ffunction-sections
         -fdata-sections
         --specs=nosys.specs
         --specs=nano.specs)
target_compile_definitions(stm32h5_hal PUBLIC -DSTM32H503xx)
target_include_directories(stm32h5_hal SYSTEM PUBLIC ./ ./cmsis/ ./Inc)
target_link_options(
  stm32h5_hal
  PUBLIC
  -mcpu=cortex-m33
  -mfloat-abi=hard
  -mfpu=fpv5-sp-d16
  -mthumb
  --specs=nosys.specs
  --specs=nano.specs)

servio_add_library(
  TARGET platform
  SOURCES adc.cpp
          clock.cpp
          uart.cpp
          config.cpp
          gpio.cpp
          i2c.cpp
          timers.cpp
          dts.cpp
  LIBS emlabcpp stm32h5_hal vari avakar::atom cfglib
  OPTS -Os)
