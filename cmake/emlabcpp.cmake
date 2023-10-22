if(NOT TARGET emlabcpp)
  include(FetchContent)
  FetchContent_Declare(
    emlabcpp
    SYSTEM
    GIT_REPOSITORY https://github.com/koniarik/emlabcpp
    GIT_TAG origin/main
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE)
  FetchContent_MakeAvailable(emlabcpp)
endif()
target_compile_options(emlabcpp PRIVATE -Os)
