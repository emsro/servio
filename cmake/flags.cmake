function(servio_compile_options target)
  set_target_properties(${target} PROPERTIES CXX_STANDARD 20 CXX_EXTENSIONS OFF)
  target_compile_options(
    ${target}
    PRIVATE -Wall
            # -Werror
            -Wextra
            -Wpedantic
            -Wunused
            -Wnull-dereference
            -Wformat=2
            -Wduplicated-cond
            -Wlogical-op # -Wlifetime
            -Wunreachable-code
            -Wsign-conversion
            -Wdouble-promotion
            -Wno-psabi
            -Wno-format-nonliteral
            -Wno-null-dereference
            $<$<COMPILE_LANGUAGE:CXX>:
            -Wconversion
            -Wuseless-cast
            # -Wcast-align ## can't get rid of it at certain places
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Woverloaded-virtual
            -Wno-mismatched-new-delete # triggers false positive on coroutines
            >)
endfunction()
