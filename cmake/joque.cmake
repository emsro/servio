if(NOT TARGET joque)
  include(FetchContent)
  FetchContent_Declare(
    joque
    SYSTEM
    GIT_REPOSITORY https://github.com/koniarik/joque
    GIT_TAG origin/main
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE)
  FetchContent_MakeAvailable(joque)
endif()
