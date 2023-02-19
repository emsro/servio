if(NOT TARGET emlabcpp)
  if(SERVIO_EMLABCPP_SRC)
    if(NOT IS_ABSOLUTE ${SERVIO_EMLABCPP_SRC})
      set(SERVIO_EMLABCPP_SRC
          "${CMAKE_CURRENT_SOURCE_DIR}/${SERVIO_EMLABCPP_SRC}")
    endif()
    add_subdirectory(${SERVIO_EMLABCPP_SRC} emlabcpp)
  else()
    include(FetchContent)
    FetchContent_Declare(
      emlabcpp
      GIT_REPOSITORY https://github.com/koniarik/emlabcpp
      GIT_TAG main)
    FetchContent_MakeAvailable(emlabcpp)
  endif()
endif()
target_compile_options(emlabcpp PRIVATE -Os)
