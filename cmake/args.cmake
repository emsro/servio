cmake_policy(SET CMP0135 NEW)

include(FetchContent)

FetchContent_Declare(
  cpp_args
  GIT_REPOSITORY https://github.com/Taywee/args
  GIT_TAG 6.4.6)

FetchContent_MakeAvailable(cpp_args)
