function(servio_compile_options target)
  set_target_properties(${target} PROPERTIES CXX_STANDARD 23 CXX_EXTENSIONS OFF)
  target_compile_options(
    ${target}
    PRIVATE -Wall
            # -Werror
            -Wextra
            -Wpedantic
            -Wunused
            -Wnull-dereference
            -Wformat=2
            -Wunreachable-code
            -Wsign-conversion
            -Wdouble-promotion
            -Wno-psabi
            -Wno-format-nonliteral
            -Wno-null-dereference
            -Wno-missing-field-initializers
            $<$<COMPILE_LANGUAGE:CXX>:
            -Wconversion
            # -Wcast-align ## can't get rid of it at certain places
            -Woverloaded-virtual
            -Wno-mismatched-new-delete # triggers false positive on coroutines
            >)
endfunction()
