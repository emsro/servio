function(servio_add_library)
  cmake_parse_arguments(A "" "TARGET" "INCLUDE;SOURCES;LIBS;OPTS" ${ARGN})

  add_library(${A_TARGET} STATIC ${A_SOURCES})
  target_include_directories(${A_TARGET} PUBLIC ${A_INCLUDE})
  servio_compile_options(${A_TARGET})
  target_link_libraries(${A_TARGET} PUBLIC ${A_LIBS})
  target_compile_options(${A_TARGET} PRIVATE ${A_OPTS})

endfunction()

function(servio_add_board_library)
  cmake_parse_arguments(A "" "TARGET" "INCLUDE;SOURCES;LIBS;OPTS" ${ARGN})

  servio_add_library(
    TARGET ${A_TARGET}
    INCLUDE ${A_INCLUDE}
    SOURCES ${A_SOURCES}
    LIBS ${A_LIBS}
         CMSIS::STM32::G431xx
         HAL::STM32::G4::CORTEX
         HAL::STM32::G4::OPAMP
         HAL::STM32::G4::PWR
         HAL::STM32::G4::PWREx
         HAL::STM32::G4::RCC
         HAL::STM32::G4::RCCEx
         STM32::NoSys
    OPTS ${A_OPTS})
endfunction()

function(servio_add_board_executable)
  cmake_parse_arguments(A "" "TARGET;LINKER_SCRIPT" "INCLUDE;SOURCES;LIBS;OPTS"
                        ${ARGN})

  add_executable(${A_TARGET} ${A_SOURCES})
  target_include_directories(${A_TARGET} PUBLIC ${A_INCLUDE})
  servio_compile_options(${A_TARGET})
  target_link_libraries(${A_TARGET} PUBLIC ${A_LIBS})
  target_compile_options(${A_TARGET} PRIVATE ${A_OPTS})

  stm32_add_linker_script(${A_TARGET} PRIVATE ${A_LINKER_SCRIPT})
  stm32_print_size_of_target(${A_TARGET})
endfunction()
