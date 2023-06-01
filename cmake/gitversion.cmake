find_file(
  GITDIR
  NAMES .git
  PATHS ${CMAKE_CURRENT_SOURCE_DIR}
  NO_DEFAULT_PATH)
set(SERVIO_GIT_DIR
    ${GITDIR}
    CACHE STRING "Git head tracking file")

# got inspired from:
# https://stackoverflow.com/questions/1435953/how-can-i-pass-git-sha1-to-compiler-as-definition-using-cmake

find_program(GIT_SCM git REQUIRED)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gitrevision.o
  COMMAND ${GIT_SCM} describe --tags >> gitrevision
  COMMAND ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/gitrevision.o
          gitrevision
  DEPENDS ${SERVIO_GIT_DIR}/logs/HEAD
  VERBATIM)
