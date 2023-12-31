if(${SERVIO_PLATFORM} STREQUAL "host")

  find_package(Protobuf CONFIG REQUIRED)

  add_library(proto io.proto)
  target_link_libraries(proto PUBLIC protobuf::libprotobuf)
  set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
  target_include_directories(proto
                             PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")

  protobuf_generate(TARGET proto PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")

elseif(${SERVIO_PLATFORM} STREQUAL "stm32g4" OR ${SERVIO_PLATFORM} STREQUAL
                                                "stm32h5")

  nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS io.proto)

  add_library(proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})
  target_link_libraries(proto PUBLIC nanopb_lib platform)
  target_include_directories(proto SYSTEM PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
  target_compile_options(proto PRIVATE -Os)

endif()
