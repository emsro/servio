include(FetchContent)

FetchContent_Declare(
  magic_enum_download
  SYSTEM
  GIT_REPOSITORY https://github.com/Neargye/magic_enum
  GIT_TAG v0.9.3
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(magic_enum_download)