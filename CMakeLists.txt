cmake_minimum_required(VERSION 3.24)

project(servio C CXX ASM)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

cmake_policy(SET CMP0077 NEW)
option(SERVIO_PLATFORM "Target platform")
option(SERVIO_EMLABCPP_SRC "Gives a location of emlabcpp source")

include(cmake/emlabcpp.cmake)
include(cmake/flags.cmake)
include(cmake/utils.cmake)
include(cmake/magic_enum.cmake)
include(cmake/nanopb.cmake)

if(${SERVIO_PLATFORM} STREQUAL "host")
  target_compile_options(
    emlabcpp PUBLIC -DEMLABCPP_USE_LOGGING -DEMLABCPP_USE_NLOHMANN_JSON
                    -DEMLABCPP_USE_GTEST)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32g4")
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fsingle-precision-constant  -Fno-rtti -fno-exceptions -gdwarf-2"
  )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsingle-precision-constant -gdwarf-2")
elseif(${SERVIO_PLATFORM} STREQUAL "stm32h5")
  set(SERVIO_SHARED_FLAGS "-fsingle-precision-constant -gdwarf-2")
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} ${SERVIO_SHARED_FLAGS} -Fno-rtti -fno-exceptions -gdwarf-2"
  )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SERVIO_SHARED_FLAGS}")
endif()

include(cmake/cmake-git-version-tracking.cmake)

if(${SERVIO_PLATFORM} STREQUAL "host")

  find_package(GTest REQUIRED)

  include(cmake/cli11.cmake)

  include(CTest)
  enable_testing()

  add_subdirectory(src/plt/host)
  add_subdirectory(src)

elseif(${SERVIO_PLATFORM} STREQUAL "stm32g4")

  stm32_fetch_hal(G4)
  stm32_fetch_cmsis(G4)
  find_package(CMSIS COMPONENTS STM32G4 REQUIRED GLOBAL)
  find_package(HAL COMPONENTS STM32G4 REQUIRED GLOBAL)

  add_subdirectory(src/plt/stm32g4)
  add_subdirectory(boards/prototype2)
  add_subdirectory(src)

  target_link_libraries(cmake_git_version_tracking PRIVATE platform)

  servio_add_board_executable(
    TARGET prototype2_fw
    SOURCES src/fw/main.cpp src/fw/it.cpp
    LIBS prototype2_board serviolib
    LINKER_SCRIPT
      "${CMAKE_CURRENT_SOURCE_DIR}/src/plt/stm32g4/STM32G431KBTx_FLASH.ld")

elseif(${SERVIO_PLATFORM} STREQUAL "stm32h5")

  stm32_fetch_hal(H5)
  stm32_fetch_cmsis(H5)
  find_package(CMSIS COMPONENTS STM32H5 REQUIRED GLOBAL)
  find_package(HAL COMPONENTS STM32H5 REQUIRED GLOBAL)

  add_subdirectory(src/plt/stm32h5)
  add_subdirectory(boards/proto3)
  add_subdirectory(src)

  target_link_libraries(cmake_git_version_tracking PRIVATE platform)

  servio_add_board_executable(
    TARGET proto3_fw
    SOURCES src/fw/main.cpp src/fw/it.cpp
    LIBS proto3_board serviolib
    LINKER_SCRIPT
      "${CMAKE_CURRENT_SOURCE_DIR}/src/plt/stm32h5/STM32H503xx_FLASH.ld")

else()
  message(WARNING "unsupported platform ${SERVIO_PLATFORM}")
endif()
