# Remove unnecessary MSVC defaults. CMake automatically adds RTTI, exception
# handling, run-time checking, and inline control flags to the compiler flag
# list. It also automatically adds incremental linking flags to the linker flag
# list. These can cause spurious warnings when these flags are later overridden
# by target-specific flags. They can also cause suboptimal code generation.
#
# This must be run after `project()` has been called.
function(sqt_clean_msvc_flags)
    foreach(lang C CXX)
        get_cmake_property(vars VARIABLES)
        list(FILTER vars INCLUDE REGEX "CMAKE_${lang}_FLAGS.*")
        list(FILTER vars EXCLUDE REGEX ".*_INIT")
        foreach(flagvarname ${vars})
            set(flags ${${flagvarname}})
            string(REPLACE "/MDd" "" flags "${flags}")
            string(REPLACE "/MD" "" flags "${flags}")
            string(REPLACE "/MT" "" flags "${flags}")
            string(REPLACE "/MTd" "" flags "${flags}")
            string(REPLACE "-MDd" "" flags "${flags}")
            string(REPLACE "-MD" "" flags "${flags}")
            string(REPLACE "-MT" "" flags "${flags}")
            string(REPLACE "-MTd" "" flags "${flags}")
            string(REPLACE "/GR" "" flags "${flags}")
            string(REPLACE "/EHsc" "" flags "${flags}")
            string(REPLACE "/RTC1" "" flags "${flags}")
            string(REPLACE "/Ob1" "" flags "${flags}")
            string(REPLACE "/Ob2" "" flags "${flags}")
            set(${flagvarname} "${flags}" PARENT_SCOPE)
            set(${flagvarname} "${flags}" CACHE STRING "Flags used by the compiler." FORCE)
        endforeach()
    endforeach()

    get_cmake_property(vars VARIABLES)
    list(FILTER vars INCLUDE REGEX "CMAKE_.*_LINKER_FLAGS_.*")
    list(FILTER vars EXCLUDE REGEX ".*_INIT")
    foreach(flagvarname ${vars})
        set(flags ${${flagvarname}})
        string(REPLACE "/INCREMENTAL:NO" "" flags "${flags}")
        string(REPLACE "/INCREMENTAL:YES" "" flags "${flags}")
        string(REPLACE "/INCREMENTAL" "" flags "${flags}")
        set(${flagvarname} "${flags}" PARENT_SCOPE)
        set(${flagvarname} "${flags}" CACHE STRING "Flags used by the linker." FORCE)
    endforeach()
endfunction()

macro(work_around_wholly_unnecessary_cmake_convolution_before_project)
    # set(CMAKE_MSVC_RUNTIME_LIBRARY "")
    cmake_policy(SET CMP0092 NEW)
    cmake_policy(SET CMP0091 NEW)

    # See https://gitlab.kitware.com/cmake/cmake/-/issues/20236
    set(CMAKE_ASM_MASM_COMPILE_OPTIONS_MSVC_RUNTIME_LIBRARY_MultiThreaded "")
    set(CMAKE_ASM_MASM_COMPILE_OPTIONS_MSVC_RUNTIME_LIBRARY_MultiThreadedDLL "")
    set(CMAKE_ASM_MASM_COMPILE_OPTIONS_MSVC_RUNTIME_LIBRARY_MultiThreadedDebug "")
    set(CMAKE_ASM_MASM_COMPILE_OPTIONS_MSVC_RUNTIME_LIBRARY_MultiThreadedDebugDLL "")
endmacro()

macro(work_around_wholly_unnecessary_cmake_convolution_after_project)
    sqt_clean_msvc_flags()
    list(REMOVE_ITEM CMAKE_CXX_IMPLICIT_LINK_LIBRARIES stdc++)
endmacro()
