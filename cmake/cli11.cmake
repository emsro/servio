include(FetchContent)
FetchContent_Declare(
  cli11
  SYSTEM
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11
  GIT_TAG v2.3.2
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)

FetchContent_MakeAvailable(cli11)
