include(CMakeParseArguments)

macro(relaxed_compile_options target scope)
    if (UNIX OR MINGW)
        target_compile_options(${target} ${scope}
            -fpermissive
            -Wno-all
            -Wno-deprecated-declarations
            -Wno-error
            -Wno-extra
            -Wno-fatal-errors
            -Wno-ignored-qualifiers
            -Wno-missing-field-initializers
            -Wno-parentheses
            -Wno-reorder
            -Wno-return-type
            -Wno-shadow
            -Wno-sign-compare
            -Wno-switch
            -Wno-undef
            -Wno-unused-const-variable
            -Wno-unused-local-typedefs
            -Wno-unused-parameter
            -Wno-unused-result
            -Wno-unused-variable
            )
    endif()
endmacro()
