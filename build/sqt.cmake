# The expansion produced by ${CMAKE_CURRENT_LIST_DIR} depends on the current
# file being processed; CMAKE_CURRENT_LIST_DIR, when used in a function defined
# in sqt.cmake, will expand to the caller's script directory instead of
# sqt.cmake's script script directory. Work around this by capturing a snapshot
# of CMAKE_CURRENT_LIST_DIR here.
set(SQT_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")
include("${SQT_CMAKE_DIR}/sqt-files.cmake")

if("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
    set(SQT_COMPILER_ID "sqt-compiler-id-msvc")
    set(SQT_COMPILER_MSVC_COMPATIBLE "true")
    set(SQT_COMPILER_GCC_COMPATIBLE "false")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
    set(SQT_COMPILER_ID "sqt-compiler-id-clang-cl")
    set(SQT_COMPILER_MSVC_COMPATIBLE "true")
    set(SQT_COMPILER_GCC_COMPATIBLE "false")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    set(SQT_COMPILER_ID "sqt-compiler-id-gcc")
    set(SQT_COMPILER_MSVC_COMPATIBLE "false")
    set(SQT_COMPILER_GCC_COMPATIBLE "true")
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    set(SQT_COMPILER_ID "sqt-compiler-id-clang")
    set(SQT_COMPILER_MSVC_COMPATIBLE "false")
    set(SQT_COMPILER_GCC_COMPATIBLE "true")
else()
    set(SQT_COMPILER_ID "sqt-compiler-id-unknown")
    set(SQT_COMPILER_MSVC_COMPATIBLE "false")
    set(SQT_COMPILER_GCC_COMPATIBLE "false")
endif()

if(NOT DEFINED SQT_HOSTMOC_EXECUTABLE_SUFFIX)
    set(SQT_HOSTMOC_EXECUTABLE_SUFFIX "${CMAKE_EXECUTABLE_SUFFIX}")
endif()

function(sqt_detect_target_architecture)
    if(MSVC)
        set(CMAKE_SYSTEM_PROCESSOR ${MSVC_CXX_ARCHITECTURE_ID})
    endif()
    if(NOT SQT_TARGET_ARCH) # if SQT_TARGET_ARCH is empty (or one of the "falsey" strings)
        set(CPUTYPE ${CMAKE_SYSTEM_PROCESSOR})
        string(TOLOWER "${CPUTYPE}" CPUTYPE)
        if("${CPUTYPE}" STREQUAL "x86_64")
            set(SQT_TARGET_ARCH "sqt-arch-amd64")
        elseif("${CPUTYPE}" STREQUAL "amd64")
            set(SQT_TARGET_ARCH "sqt-arch-amd64")
        elseif("${CPUTYPE}" STREQUAL "x64")
            set(SQT_TARGET_ARCH "sqt-arch-amd64")
        elseif("${CPUTYPE}" STREQUAL "x86")
            set(SQT_TARGET_ARCH "sqt-arch-i386")
        endif()
    endif()
    message(STATUS "sqt: CMAKE_SYSTEM_PROCESSOR is \"${CMAKE_SYSTEM_PROCESSOR}\".")
    message(STATUS "sqt: setting SQT_TARGET_ARCH to \"${SQT_TARGET_ARCH}\".")
    set(SQT_TARGET_ARCH "${SQT_TARGET_ARCH}" PARENT_SCOPE)
endfunction()

function(sqt_detect_target_os)
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        set(SQT_TARGET_OS "sqt-os-windows")
        set(SQT_files_tag "win32")
        set(SQT_TARGET_OS_REQUIRES_FORKFD "FALSE")
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
        set(SQT_TARGET_OS "sqt-os-macosx")
        set(SQT_files_tag "osx")
        set(SQT_TARGET_OS_REQUIRES_FORKFD "TRUE")
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        set(SQT_TARGET_OS "sqt-os-linux")
        set(SQT_files_tag "linux")
        set(SQT_TARGET_OS_REQUIRES_FORKFD "TRUE")
    elseif(UNIX)
        set(SQT_TARGET_OS "sqt-os-posix")
        set(SQT_files_tag "unix")
        set(SQT_TARGET_OS_REQUIRES_FORKFD "TRUE")
    endif()
    message(STATUS "sqt: CMAKE_SYSTEM_NAME is \"${CMAKE_SYSTEM_NAME}\".")
    message(STATUS "sqt: setting SQT_TARGET_OS to \"${SQT_TARGET_OS}\".")
    message(STATUS "sqt: setting SQT_TARGET_OS_REQUIRES_FORKFD to \"${SQT_TARGET_OS_REQUIRES_FORKFD}\".")
    set(SQT_TARGET_OS "${SQT_TARGET_OS}" PARENT_SCOPE)
    set(SQT_files_tag "${SQT_files_tag}" PARENT_SCOPE)
    set(SQT_TARGET_OS_REQUIRES_FORKFD "${SQT_TARGET_OS_REQUIRES_FORKFD}" PARENT_SCOPE)
endfunction()

macro(sqt_detect_target)
    sqt_detect_target_architecture()
    sqt_detect_target_os()
endmacro()

function(sqt_append_mkspecs includes)
    set(mkspecs--sqt-os-windows--sqt-compiler-id-msvc "src/qt/qtbase/mkspecs/win32-msvc")
    set(mkspecs--sqt-os-windows--sqt-compiler-id-clang "src/qt/qtbase/mkspecs/win32-clang-g++")
    set(mkspecs--sqt-os-windows--sqt-compiler-id-gcc "src/qt/qtbase/mkspecs/win32-g++")

    set(mkspecs--sqt-os-linux--sqt-compiler-id-clang "src/qt/qtbase/mkspecs/linux-clang")
    set(mkspecs--sqt-os-linux--sqt-compiler-id-gcc "src/qt/qtbase/mkspecs/linux-g++")

    if(DEFINED mkspecs--${SQT_TARGET_OS}--${SQT_COMPILER_ID})
        list(APPEND ${includes} ${mkspecs--${SQT_TARGET_OS}--${SQT_COMPILER_ID}})
    else()
        message(FATAL_ERROR "sqt: No mkspecs found for ${SQT_TARGET_OS} with ${SQT_COMPILER_ID}.")
    endif()


    set(${includes} ${${includes}} PARENT_SCOPE)
endfunction()

function(sqt_append_subdir_private_include includes subdir qt_module_name)
    list(APPEND ${includes}
        "src/qt/${subdir}/include/${qt_module_name}"
        "src/qt/${subdir}/include/${qt_module_name}/5.15.2"
        "src/qt/${subdir}/include/${qt_module_name}/5.15.2/${qt_module_name}"
        "src/qt/${subdir}/include/${qt_module_name}/5.15.2/${qt_module_name}/private")
    if(SQT_TARGET_OS_REQUIRES_FORKFD)
        list(APPEND ${includes} "src/qt/qtbase/src/3rdparty/forkfd")
    endif()
    set(${includes} ${${includes}} PARENT_SCOPE)
endfunction()

macro(sqt_append_private_include includes qt_module_name)
    sqt_append_subdir_private_include("${includes}" "qtbase" "${qt_module_name}")
endmacro()

function(sqt_adjust_file_lists prefix)
    foreach(module ${SQT_ALL_MODULES})
        foreach(os generic win32 linux unix osx)
            set(xs "${SQT_FILES_${module}_${os}}")
            list(FILTER xs EXCLUDE REGEX ".*VERSIONTAGGING_SOURCES.*")
            list(FILTER xs EXCLUDE REGEX ".*/rhi/.*")
            list(FILTER xs EXCLUDE REGEX ".*/webgradients")
            list(FILTER xs EXCLUDE REGEX ".*/qshadergenerator.*")
            list(FILTER xs EXCLUDE REGEX ".*/qshaderlanguage.*")
            list(FILTER xs EXCLUDE REGEX ".*/qshader.*")
            list(FILTER xs EXCLUDE REGEX ".*PUBLIC_HEADERS.*")
            list(FILTER xs EXCLUDE REGEX ".*PRIVATE_HEADERS.*")
            list(FILTER xs EXCLUDE REGEX ".*/..XCB_DIR.*")
            list(TRANSFORM xs PREPEND "${prefix}/")
            set(SQT_FILES_${module}_${os} ${xs} PARENT_SCOPE)
        endforeach()

        set(xs ${SQT_INCLUDES_${module}})
        list(TRANSFORM xs PREPEND "${prefix}/")
        list(APPEND xs ${SQT_INCLUDES_ABSOLUTE_${module}})
        set(SQT_INCLUDES_${module} ${xs} PARENT_SCOPE)
    endforeach()
endfunction()

function(sqt_target_add_standard_flags target)
    if("${SQT_COMPILER_MSVC_COMPATIBLE}")
        target_compile_options("${target}" PRIVATE
            $<$<COMPILE_LANGUAGE:CXX,C>:/GF> # Deduplicate strings.
            $<$<COMPILE_LANGUAGE:CXX,C>:/GS-> # Disable the MS ABI buffer security check.
            $<$<COMPILE_LANGUAGE:CXX,C>:/GR-> # Disable C++ RTTI.
            $<$<COMPILE_LANGUAGE:CXX,C>:/Gy> # Like -ffunction-sections for cl. Prerequisite for /OPT:REF dead code elimination.
            $<$<COMPILE_LANGUAGE:CXX,C>:/Gw> # Like -fdata-sections for cl. Prerequisite for /OPT:REF dead data elimination.
            $<$<COMPILE_LANGUAGE:CXX,C>:/Oi> # Generate calls to intrinsic functions.
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:threadSafeInit-> # Disable C++11 "thread safe statics".
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zo> # "Enhance Optimized Debugging"
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:inline> # Remove unreferenced functions/data
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:ternary>
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:forScope>
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:rvalueCast>
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:wchar_t>
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:strictStrings>
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:__cplusplus>
            # $<$<COMPILE_LANGUAGE:CXX,C>:/bigobj>
            $<$<COMPILE_LANGUAGE:CXX,C>:/wd4996> # Disables deprecation warnings. QLinkedList is deprecated but is used anyway.
            )
        target_compile_definitions("${target}" PRIVATE "_CRT_SECURE_NO_WARNINGS=1")
        if("${SQT_TARGET_ARCH}" STREQUAL "sqt-arch-i386")
            target_compile_options("${target}" PRIVATE "/arch:IA32")
        endif()
        get_target_property(use_msvcrt "${target}" SQT_USE_MSVCRT)
        if(${use_msvcrt})
            target_compile_definitions("${target}" PRIVATE
                "_ITERATOR_DEBUG_LEVEL=0"
                "_ACRTIMP_ALT="
                "_HAS_EXCEPTIONS=0"
                "_NO_CRT_STDIO_INLINE=1")
            target_link_options("${target}" PRIVATE
                "/ignore:4286" # __declspec(dllimport) specified on a symbol that is defined in the same image
                "/ignore:4217" # __declspec(dllimport) specified on a symbol that is defined in the same image
                "/ignore:4049" # locally defined symbol ... imported
                )
            set_target_properties("${target}" PROPERTIES
                MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
        endif()
    elseif("${SQT_COMPILER_GCC_COMPATIBLE}")
        target_compile_options("${target}" PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
            $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
            $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
            $<$<COMPILE_LANGUAGE:CXX,C>:-fdata-sections>
            $<$<COMPILE_LANGUAGE:CXX,C>:-ffunction-sections>
            $<$<COMPILE_LANGUAGE:CXX,C>:-fvisibility=hidden>
            $<$<COMPILE_LANGUAGE:CXX,C>:-Wno-deprecated-declarations> # QLinkedList is deprecated but is used anyway.
            $<$<COMPILE_LANGUAGE:CXX,C>:-fvisibility=hidden>
            $<$<COMPILE_LANGUAGE:CXX,C>:-fno-asynchronous-unwind-tables>
            )
        if("${SQT_TARGET_OS}" STREQUAL "sqt-os-windows")
            target_compile_options("${target}" PRIVATE
                $<$<COMPILE_LANGUAGE:CXX,C>:-Wa,-mbig-obj>) # Some of the object files generated by Qt contain more than 32768 symbols, so use "big" PECOFF object files.
        elseif("${SQT_TARGET_OS}" STREQUAL "sqt-os-linux")
            target_compile_options("${target}" PRIVATE
                $<$<COMPILE_LANGUAGE:CXX,C>:-fno-semantic-interposition>
                $<$<COMPILE_LANGUAGE:CXX,C>:-fstack-clash-protection>
                $<$<COMPILE_LANGUAGE:CXX,C>:-fno-plt> # Lazy binding and interposition are stupid.
                )
        endif()
    endif()

    if("${SQT_COMPILER_ID}" STREQUAL "sqt-compiler-id-msvc")
        target_compile_options("${target}" PRIVATE
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:referenceBinding>)
    endif()

    target_compile_definitions("${target}" PRIVATE "QT_NO_EXCEPTIONS")

    get_target_property(dont-strip "${target}" sqt-target-prop-dont-strip-versioned-symbols)
    get_target_property(target_type "${target}" TYPE)
    if("${target_type}" MATCHES "^(EXECUTABLE|SHARED_LIBRARY)$")
        if("${SQT_TARGET_OS}" STREQUAL "sqt-os-linux")
            if(NOT dont-strip)
                #add_custom_command(TARGET "${target}" POST_BUILD
                #    COMMAND ${CMAKE_OBJCOPY} --remove-section .gnu.version --remove-section .gnu.version_r $<TARGET_FILE:${target}>
                #    COMMAND ${CMAKE_BINARY_DIR}/bootstrap/strip-versioned-symbols${SQT_HOSTMOC_EXECUTABLE_SUFFIX} $<TARGET_FILE:${target}>
                #    )
            endif()
            target_link_options("${target}" PRIVATE
                # Only added DT_NEEDED dependencies for libraries that are
                # actually used by the output binary.
                "-Wl,--as-needed"

                # Mark pages modified by the dynamic loader during relocation as
                # read-only after the loader has finished relocation.
                "-Wl,-z,relro"

                # Disable lazy symbol resolution.
                "-Wl,-z,now"

                # In a .so, if a .so-local definition for a global symbol exists,
                # use it instead of using a call through the PLT or GOT or
                # whatever. This mirrors the behavior of linkers and DLLs on
                # Windows. Note that it's possible for multiple different .sos to
                # include definitions of symbols that are assumed to be
                # whole-program-global, like RTTI information, which is used in
                # the implementation of exceptions. But this doesn't matter to
                # me, and shouldn't matter for any well-designed project: the very
                # idea of a "stable C++ ABI" is silly, and letting exceptions or
                # whatever implicitly-generated uncontrollable C++ crap leak across
                # .so/DLL boundaries is a bad idea.
                "-Wl,-Bsymbolic"
                "-Wl,-Bsymbolic-functions"

                "-Wl,--no-undefined"

                "-Wl,--build-id=0x0101010101010102"
                )
        endif()

        if(SQT_COMPILER_MSVC_COMPATIBLE)
            get_target_property(msvc-keep-runtime-library "${target}" sqt-target-prop-msvc-keep-runtime-library)
            if(NOT msvc-keep-runtime-library)
                target_compile_options("${target}" PRIVATE
                    $<$<COMPILE_LANGUAGE:CXX,C>:/Zl>)
            endif()
            target_link_options("${target}" PRIVATE
                "/INCREMENTAL:NO" # Disables incremental linking. Incremental linking can add extra padding and extra thunks.
                "/OPT:REF" # Dead reference elimination.
                "/OPT:ICF" # COMDAT folding.
                )
            add_custom_command(TARGET "${target}" POST_BUILD
                COMMAND editbin.exe /subsystem:WINDOWS,3.10 /osversion:3.1 $<TARGET_FILE:${target}>
                )
            if("${SQT_TARGET_ARCH}" STREQUAL "sqt-arch-i386")
                target_link_options("${target}" PRIVATE
                    "/LARGEADDRESSAWARE" # Marks 32-bit binaries as capable of using a full 4GiB of address space.
                )
            endif()
        elseif(SQT_COMPILER_GCC_COMPATIBLE)
            add_custom_command(TARGET "${target}" POST_BUILD
                COMMAND ${CMAKE_OBJCOPY} --only-keep-debug $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>.dbg
                COMMAND ${CMAKE_OBJCOPY} --strip-all $<TARGET_FILE:${target}>
                COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:${target}>.dbg $<TARGET_FILE:${target}>)
            target_link_options("${target}" PRIVATE
                "-Wl,--gc-sections" # Dead code elimination.
                )
            if("${SQT_TARGET_ARCH}" MATCHES "^(sqt-arch-i386|sqt-arch-amd64)$")
                target_link_options("${target}" PRIVATE
                    # On i386: Qt depends on 64-bit multiplication and division,
                    # which apparently depends on some supporting procedures
                    # defined in libgcc. Statically link libgcc here instead of
                    # rebuilding the support functions from scratch.
                    #
                    # On amd64: Qt depends on __popcountdi2. Instead of figuring
                    # out how to work around this, just statically link it in.
                    "-static-libgcc"
                    )
            endif()
            if("${SQT_TARGET_OS}" STREQUAL "sqt-os-windows")
                target_link_options("${target}" PRIVATE
                    "-Wl,--major-subsystem-version,3"
                    "-Wl,--minor-subsystem-version,10"
                    "-Wl,--major-os-version,3"
                    "-Wl,--minor-os-version,10")
                if("${SQT_TARGET_ARCH}" STREQUAL "sqt-arch-i386")
                    target_link_options("${target}" PRIVATE
                        "-Wl,--large-address-aware" # Marks 32-bit binaries as capable of using a full 4GiB of address space.
                        )
                endif()
            endif()
        endif()

    endif()
endfunction()


function(sqt_add_bootstrap_definitions target)
    target_compile_definitions("${target}" PRIVATE
        "QT_NO_DEBUG"
        "QT_USE_QSTRINGBUILDER"
        "QT_BUILDING_QT"

        "QT_BOOTSTRAPPED"
        "QT_NO_FOREACH"
        "QT_NO_EXCEPTIONS"
        "QT_STATIC"
        "QT_VERSION_MAJOR=5"
        "QT_VERSION_MINOR=15"
        "QT_VERSION_PATCH=2"
        "QT_VERSION_STR=\"5.15.2\"")
endfunction()

function(sqt_use_moc target)
    add_dependencies("${target}" "bootstrap-executables")
    set_target_properties("${target}" PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
        AUTOMOC_EXECUTABLE "${CMAKE_BINARY_DIR}/bootstrap/moc${SQT_HOSTMOC_EXECUTABLE_SUFFIX}"
        AUTOUIC_EXECUTABLE "${CMAKE_BINARY_DIR}/bootstrap/uic${SQT_HOSTMOC_EXECUTABLE_SUFFIX}"
        AUTORCC_EXECUTABLE "${CMAKE_BINARY_DIR}/bootstrap/rcc${SQT_HOSTMOC_EXECUTABLE_SUFFIX}")
endfunction()

function(sqt_add_platform_definitions target)
    target_compile_definitions("${target}" PRIVATE
        "NDEBUG"
        $<$<PLATFORM_ID:Windows>:UNICODE>
        $<$<PLATFORM_ID:Windows>:WIN32>
        $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
        $<$<PLATFORM_ID:Windows>:_ENABLE_EXTENDED_ALIGNED_STORAGE>
        $<$<PLATFORM_ID:Windows>:_UNICODE>
        $<$<PLATFORM_ID:Windows>:_WIN32>
        $<$<PLATFORM_ID:Windows>:WINVER=0x0602>
        $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0602>
        $<$<PLATFORM_ID:Windows>:_WIN32_IE=0x0700>
        $<$<PLATFORM_ID:Windows>:NTDDI_VERSION=0x06010000>)
endfunction()

function(sqt_glob_file_list dstvar prefix)
    set(files ${ARGN})
    list(TRANSFORM files PREPEND "${prefix}/")
    file(GLOB_RECURSE files ${files})
    set(${dstvar} ${files} PARENT_SCOPE)
endfunction()

function(def_info_bootstrap)
    set(SQT_INCLUDES_bootstrap
        "src/stubs/config-include"
        "src/stubs"
        "src/qt/qtbase/include")
    sqt_append_private_include(SQT_INCLUDES_bootstrap "QtCore")
    sqt_append_private_include(SQT_INCLUDES_bootstrap "QtXml")
    list(APPEND SQT_INCLUDES_bootstrap
        "src/qt/qtbase/include/QtZlib"
        "src/qt/qtbase/src/3rdparty/tinycbor/src"
        )
    sqt_append_mkspecs(SQT_INCLUDES_bootstrap)

    list(APPEND SQT_FILES_bootstrap_generic
        "src/stubs/c++-stubs.cpp")

    list(APPEND SQT_FILES_bootstrap_win32
        "src/stubs/windows/windows-stubs.hpp"
        "src/stubs/windows/windows-stubs-gen.hpp"
        "src/stubs/windows/windows-stubs-advapi32.cpp"
        "src/stubs/windows/windows-stubs-kernel32.cpp"
        "src/stubs/windows/windows-stubs-shell32.cpp"
        )

    set(SQT_FILES_bootstrap_generic ${SQT_FILES_bootstrap_generic} PARENT_SCOPE)
    set(SQT_FILES_bootstrap_win32 ${SQT_FILES_bootstrap_win32} PARENT_SCOPE)
    set(SQT_INCLUDES_bootstrap ${SQT_INCLUDES_bootstrap} PARENT_SCOPE)
endfunction()

function(def_info_corelib)
    set(SQT_INCLUDES_corelib
        "src/stubs/config-include"
        "src/stubs/config-include/QtCore"
        "src/stubs"
        "src/qt/qtbase/include")
    sqt_append_private_include(SQT_INCLUDES_corelib "QtCore")
    list(APPEND SQT_INCLUDES_corelib
        "src/qt/qtbase/src/3rdparty"
        "src/qt/qtbase/src/3rdparty/double-conversion/include"
        "src/qt/qtbase/src/3rdparty/harfbuzz/src"
        "src/qt/qtbase/src/3rdparty/pcre2/src"
        "src/qt/qtbase/src/3rdparty/tinycbor/src"
        "src/qt/qtbase/src/3rdparty/zlib/src")
    sqt_append_mkspecs(SQT_INCLUDES_corelib)

    set(SQT_LINKS_corelib "lib-pcre2")
    if("${SQT_TARGET_OS}" STREQUAL "sqt-os-windows")
        list(APPEND SQT_LINKS_corelib "ws2_32" "version" "netapi32" "userenv" "winmm" "mpr")
    elseif("${SQT_TARGET_OS}" STREQUAL "sqt-os-linux")
        list(APPEND SQT_LINKS_corelib "Threads::Threads" ${CMAKE_DL_LIBS})
    endif()

    list(APPEND SQT_FILES_corelib_generic
        "src/stubs/c++-stubs.cpp")

    list(APPEND SQT_FILES_corelib_win32
        "src/stubs/windows/windows-stubs.hpp"
        "src/stubs/windows/windows-stubs-gen.hpp"
        "src/stubs/windows/windows-stubs-advapi32.cpp"
        "src/stubs/windows/windows-stubs-kernel32.cpp"
        "src/stubs/windows/windows-stubs-shell32.cpp"
        "src/stubs/windows/windows-stubs-user32.cpp"
        )

    set(SQT_FILES_corelib_generic ${SQT_FILES_corelib_generic} PARENT_SCOPE)
    set(SQT_FILES_corelib_win32 ${SQT_FILES_corelib_win32} PARENT_SCOPE)
    set(SQT_INCLUDES_corelib ${SQT_INCLUDES_corelib} PARENT_SCOPE)
    set(SQT_DEFINES_corelib
        "PCRE2_CODE_UNIT_WIDTH=16"
        "PCRE2_STATIC"
        "QT_USE_QSTRINGBUILDER"
        "QT_BUILDING_QT"
        "QT_NO_EXCEPTIONS"
        "QT_BUILD_CORE_LIB"
        "QT_NO_DEBUG"
        "QT_SHARED"
        PARENT_SCOPE)
    set(SQT_LINKS_corelib ${SQT_LINKS_corelib} PARENT_SCOPE)
endfunction()

function(def_info_gui)
    set(SQT_INCLUDES_gui
        "src/stubs/config-include"
        "src/stubs/config-include/QtCore"
        "src/stubs"
        "src/qt/qtbase/include")
    sqt_append_private_include(SQT_INCLUDES_gui "QtCore")
    sqt_append_private_include(SQT_INCLUDES_gui "QtGui")
    list(APPEND SQT_INCLUDES_gui
        "src/qt/qtbase/src/3rdparty"
        "src/qt/qtbase/src/3rdparty/double-conversion/include"
        "src/qt/qtbase/src/3rdparty/harfbuzz-ng/include"
        "src/qt/qtbase/src/3rdparty/harfbuzz/src"
        "src/qt/qtbase/src/3rdparty/libpng"
        "src/qt/qtbase/src/3rdparty/pcre2/src"
        "src/qt/qtbase/src/3rdparty/tinycbor/src"
        "src/qt/qtbase/src/3rdparty/zlib/src")
    sqt_append_mkspecs(SQT_INCLUDES_gui)

    list(APPEND SQT_FILES_gui_generic "src/stubs/qmake_webgradients.qrc")
    if(("${SQT_TARGET_ARCH}" STREQUAL "sqt-arch-amd64") OR ("${SQT_TARGET_ARCH}" STREQUAL "sqt-arch-i386"))
        list(APPEND SQT_FILES_gui_generic
            "src/qt/qtbase/src/gui/image/qimage_ssse3.cpp"
            "src/qt/qtbase/src/gui/painting/qdrawhelper_avx2.cpp"
            "src/qt/qtbase/src/gui/painting/qdrawhelper_sse2.cpp"
            "src/qt/qtbase/src/gui/painting/qdrawhelper_sse4.cpp"
            "src/qt/qtbase/src/gui/painting/qdrawhelper_ssse3.cpp"
            "src/qt/qtbase/src/gui/painting/qimagescale_sse4.cpp")
    endif()

    set(SQT_LINKS_gui "lib-png" "lib-harfbuzz")
    if("${SQT_TARGET_OS}" STREQUAL "sqt-os-windows")
        list(APPEND SQT_LINKS_gui "opengl32")
    elseif("${SQT_TARGET_OS}" STREQUAL "sqt-os-linux")
        list(APPEND SQT_LINKS_gui "GL")
    endif()

    list(APPEND SQT_FILES_gui_generic
        "src/stubs/c++-stubs.cpp")

    list(APPEND SQT_FILES_gui_win32
        "src/stubs/windows/windows-stubs.hpp"
        "src/stubs/windows/windows-stubs-gen.hpp"
        "src/stubs/windows/windows-stubs-kernel32.cpp")

    set(SQT_FILES_gui_generic ${SQT_FILES_gui_generic} PARENT_SCOPE)
    set(SQT_FILES_gui_win32 ${SQT_FILES_gui_win32} PARENT_SCOPE)
    set(SQT_INCLUDES_gui ${SQT_INCLUDES_gui} PARENT_SCOPE)
    set(SQT_DEFINES_gui
        "QT_USE_QSTRINGBUILDER"
        "QT_BUILDING_QT"
        "QT_NO_EXCEPTIONS"
        "QT_BUILD_GUI_LIB"
        "QT_NO_DEBUG"
        "QT_SHARED"
        PARENT_SCOPE)
    set(SQT_LINKS_gui ${SQT_LINKS_gui} PARENT_SCOPE)
endfunction()

function(def_info_widgets)
    set(SQT_INCLUDES_widgets
        "src/stubs/config-include"
        "src/stubs/config-include/QtCore"
        "src/stubs"
        "src/qt/qtbase/include")
    sqt_append_private_include(SQT_INCLUDES_widgets "QtCore")
    sqt_append_private_include(SQT_INCLUDES_widgets "QtGui")
    sqt_append_private_include(SQT_INCLUDES_widgets "QtWidgets")
    list(APPEND SQT_INCLUDES_widgets
        "src/src/3rdparty"
        "src/src/3rdparty/double-conversion/include"
        "src/src/3rdparty/harfbuzz/src"
        "src/src/3rdparty/harfbuzz-ng/include"
        "src/src/3rdparty/pcre2/src"
        "src/src/3rdparty/tinycbor/src"
        "src/src/3rdparty/zlib/src")
    sqt_append_mkspecs(SQT_INCLUDES_widgets)

    list(APPEND SQT_FILES_widgets_generic
        "src/stubs/c++-stubs.cpp")

    list(APPEND SQT_FILES_widgets_win32
        "src/stubs/windows/windows-stubs.hpp"
        "src/stubs/windows/windows-stubs-gen.hpp"
        "src/stubs/windows/windows-stubs-kernel32.cpp"
        "src/stubs/windows/windows-stubs-shell32.cpp")

    set(SQT_FILES_widgets_generic ${SQT_FILES_widgets_generic} PARENT_SCOPE)
    set(SQT_FILES_widgets_win32 ${SQT_FILES_widgets_win32} PARENT_SCOPE)
    set(SQT_INCLUDES_widgets ${SQT_INCLUDES_widgets} PARENT_SCOPE)
    set(SQT_DEFINES_widgets
        "QT_USE_QSTRINGBUILDER"
        "QT_BUILDING_QT"
        "QT_NO_EXCEPTIONS"
        "QT_BUILD_WIDGETS_LIB"
        "QT_NO_DEBUG"
        "QT_SHARED"
        PARENT_SCOPE)
    set(SQT_LINKS_widgets PARENT_SCOPE)
endfunction()

function(def_info_network)
    set(SQT_INCLUDES_network
        "src/stubs/config-include"
        "src/stubs/config-include/QtCore"
        "src/stubs"
        "src/qt/qtbase/include"
        "src/qt/qtbase/src/network")
    sqt_append_private_include(SQT_INCLUDES_network "QtCore")
    sqt_append_private_include(SQT_INCLUDES_network "QtNetwork")
    list(APPEND SQT_INCLUDES_network
        "src/qt/qtbase/src/3rdparty/zlib/src")
    sqt_append_mkspecs(SQT_INCLUDES_network)

    set(SQT_FILES_network_generic ${SQT_FILES_network_generic} PARENT_SCOPE)
    set(SQT_FILES_network_win32 ${SQT_FILES_network_win32} PARENT_SCOPE)
    set(SQT_INCLUDES_network ${SQT_INCLUDES_network} PARENT_SCOPE)
    set(SQT_DEFINES_network
        "QT_USE_QSTRINGBUILDER"
        "QT_BUILDING_QT"
        "QT_NO_EXCEPTIONS"
        "QT_BUILD_NETWORK_LIB"
        "QT_NO_DEBUG"
        "QT_SHARED"
        PARENT_SCOPE)
    set(SQT_LINKS_network "ws2_32" "iphlpapi" PARENT_SCOPE)
endfunction()

function(def_info_websockets)
    set(SQT_INCLUDES_websockets
        "src/stubs/config-include"
        "src/stubs/config-include/QtCore"
        "src/stubs"
        "src/qt/qtbase/include"
        "src/qt/qtwebsockets/include")
    sqt_append_private_include(SQT_INCLUDES_websockets "QtCore")
    sqt_append_private_include(SQT_INCLUDES_websockets "QtNetwork")
    sqt_append_subdir_private_include(SQT_INCLUDES_websockets "qtwebsockets" "QtWebSockets")
    sqt_append_mkspecs(SQT_INCLUDES_websockets)

    set(SQT_FILES_websockets_generic ${SQT_FILES_websockets_generic} PARENT_SCOPE)
    set(SQT_FILES_websockets_win32 ${SQT_FILES_websockets_win32} PARENT_SCOPE)
    set(SQT_INCLUDES_websockets ${SQT_INCLUDES_websockets} PARENT_SCOPE)
    set(SQT_DEFINES_websockets
        "QT_USE_QSTRINGBUILDER"
        "QT_BUILDING_QT"
        "QT_NO_EXCEPTIONS"
        "QT_BUILD_WEBSOCKETS_LIB"
        "QT_NO_DEBUG"
        "QT_SHARED"
        PARENT_SCOPE)
    set(SQT_LINKS_websockets PARENT_SCOPE)
endfunction()

function(def_info_platform_windows)
    set(SQT_INCLUDES_platform_windows
        "src/stubs/config-include"
        "src/stubs/config-qwindows"
        "src/stubs"
        "src/qt/qtbase/include")
    sqt_append_private_include(SQT_INCLUDES_platform_windows "QtCore") # qglobal.h, private/qobject_p.h
    sqt_append_private_include(SQT_INCLUDES_platform_windows "QtGui") # qicon.h
    sqt_append_private_include(SQT_INCLUDES_platform_windows "QtEventDispatcherSupport")
    sqt_append_private_include(SQT_INCLUDES_platform_windows "QtFontDatabaseSupport")
    sqt_append_private_include(SQT_INCLUDES_platform_windows "QtThemeSupport")
    sqt_append_private_include(SQT_INCLUDES_platform_windows "QtWindowsUIAutomationSupport")
    list(APPEND SQT_INCLUDES_platform_windows
        "src/qt/qtbase/src/3rdparty/wintab"
        "src/qt/qtbase/src/plugins/platforms/windows" # qwindowsuiaccessibility.h
        )
    sqt_append_mkspecs(SQT_INCLUDES_platform_windows)
    set(SQT_LINKS_platform_windows "imm32" "oleaut32" "gdi32" "ole32" "winmm")
    # list(APPEND SQT_LINKS_platform_windows "wtsapi32")

    list(APPEND SQT_FILES_platform_windows_generic
        ${SQT_FILES_platform_generic_eventdispatchers_generic}
        ${SQT_FILES_platform_generic_eventdispatchers_win32}
        ${SQT_FILES_platform_generic_fontdatabases_generic}
        ${SQT_FILES_platform_generic_fontdatabases_win32}
        ${SQT_FILES_platform_generic_themes_generic}
        ${SQT_FILES_platform_generic_themes_win32}
        ${SQT_FILES_platform_windows_windowsuiautomation_generic}
        ${SQT_FILES_platform_windows_windowsuiautomation_win32})

    list(APPEND SQT_FILES_platform_windows_generic
        "src/stubs/c++-stubs.cpp")

    list(APPEND SQT_FILES_platform_windows_win32
        "src/stubs/windows/windows-stubs.hpp"
        "src/stubs/windows/windows-stubs-gen.hpp"
        "src/stubs/windows/windows-stubs-dwmapi.cpp"
        "src/stubs/windows/windows-stubs-gdi32.cpp"
        "src/stubs/windows/windows-stubs-imm32.cpp"
        "src/stubs/windows/windows-stubs-kernel32.cpp"
        "src/stubs/windows/windows-stubs-shell32.cpp"
        "src/stubs/windows/windows-stubs-user32.cpp"
        "src/stubs/windows/windows-stubs-wtsapi.cpp"
        )

    set(SQT_FILES_platform_windows_generic ${SQT_FILES_platform_windows_generic} PARENT_SCOPE)
    set(SQT_FILES_platform_windows_win32 ${SQT_FILES_platform_windows_win32} PARENT_SCOPE)
    set(SQT_INCLUDES_platform_windows ${SQT_INCLUDES_platform_windows} PARENT_SCOPE)
    set(SQT_DEFINES_platform_windows
        "QT_NO_DIRECTWRITE"
        "QT_NO_FREETYPE"
        "QT_USE_QSTRINGBUILDER"
        "QT_BUILDING_QT"
        "QT_NO_EXCEPTIONS"
        "QT_NO_DEBUG"
        PARENT_SCOPE)
    set(SQT_LINKS_platform_windows ${SQT_LINKS_platform_windows} PARENT_SCOPE)
endfunction()

include(FindFreetype)

function(def_info_platform_xcb)
    set(SQT_INCLUDES_platform_xcb
        "src/stubs/config-include"
        "src/stubs"
        "src/qt/qtbase/include"
        "src/qt/qtbase/src/plugins/platforms/xcb"
        "src/qt/qtbase/src/plugins/platforms/xcb/gl_integrations")
    sqt_append_mkspecs(SQT_INCLUDES_platform_xcb)
    sqt_append_private_include(SQT_INCLUDES_platform_xcb "QtCore") # qglobal.h, private/qobject_p.h
    sqt_append_private_include(SQT_INCLUDES_platform_xcb "QtGui") # qicon.h
    sqt_append_private_include(SQT_INCLUDES_platform_xcb "QtEdidSupport") # qedidparser_p.h
    sqt_append_private_include(SQT_INCLUDES_platform_xcb "QtXkbCommonSupport") # qxkbcommon_p.h
    sqt_append_private_include(SQT_INCLUDES_platform_xcb "QtFontDatabaseSupport") # qgenericunixfontdatabase_p.h
    sqt_append_private_include(SQT_INCLUDES_platform_xcb "QtServiceSupport") # qgenericunixservices_p.h
    sqt_append_private_include(SQT_INCLUDES_platform_xcb "QtThemeSupport") # qgenericunixthemes_p.
    sqt_append_private_include(SQT_INCLUDES_platform_xcb "QtLinuxAccessibilitySupport") # bridge_p.h
    set(SQT_INCLUDES_ABSOLUTE_platform_xcb ${FREETYPE_INCLUDE_DIRS})

    set(SQT_LINKS_platform_xcb
        "xcb"
        "xcb-icccm"
        "xcb-image"
        "xcb-keysyms"
        "xcb-randr"
        "xcb-render"
        "xcb-render-util"
        "xcb-shape"
        "xcb-shm"
        "xcb-sync"
        "xcb-xfixes"
        "xcb-xinerama"
        "xcb-xinput"
        "xcb-xkb"

        "X11"
        "X11-xcb"

        "xkbcommon" "xkbcommon-x11"

        "freetype"
        "fontconfig"
        )

    list(APPEND SQT_FILES_platform_xcb_generic
        ${SQT_FILES_platform_xcb_qpa_generic}

        "src/qt/qtbase/src/platformsupport/services/genericunix/qgenericunixservices_p.h"
        "src/qt/qtbase/src/platformsupport/services/genericunix/qgenericunixservices.cpp"

        "src/qt/qtbase/src/platformsupport/fontdatabases/fontconfig/qfontconfigdatabase_p.h"
        "src/qt/qtbase/src/platformsupport/fontdatabases/fontconfig/qfontenginemultifontconfig_p.h"
        "src/qt/qtbase/src/platformsupport/fontdatabases/fontconfig/qfontconfigdatabase.cpp"
        "src/qt/qtbase/src/platformsupport/fontdatabases/fontconfig/qfontenginemultifontconfig.cpp"

        "src/qt/qtbase/src/platformsupport/fontdatabases/freetype/qfontengine_ft_p.h"
        "src/qt/qtbase/src/platformsupport/fontdatabases/freetype/qfreetypefontdatabase_p.h"
        "src/qt/qtbase/src/platformsupport/fontdatabases/freetype/qfontengine_ft.cpp"
        "src/qt/qtbase/src/platformsupport/fontdatabases/freetype/qfreetypefontdatabase.cpp"

        "src/qt/qtbase/src/platformsupport/edid/qedidparser_p.h"
        "src/qt/qtbase/src/platformsupport/edid/qedidparser.cpp"

        "src/qt/qtbase/src/platformsupport/input/xkbcommon/qxkbcommon_p.h"
        "src/qt/qtbase/src/platformsupport/input/xkbcommon/qxkbcommon.cpp"
        "src/qt/qtbase/src/platformsupport/input/xkbcommon/qxkbcommon_3rdparty.cpp"

        "src/qt/qtbase/src/platformsupport/themes/genericunix/qgenericunixthemes_p.h"
        "src/qt/qtbase/src/platformsupport/themes/genericunix/qgenericunixthemes.cpp"
        )

    list(APPEND SQT_FILES_platform_xcb_generic
        "src/stubs/c++-stubs.cpp")

    set(SQT_FILES_platform_xcb_generic ${SQT_FILES_platform_xcb_generic} PARENT_SCOPE)
    set(SQT_INCLUDES_platform_xcb ${SQT_INCLUDES_platform_xcb} PARENT_SCOPE)
    set(SQT_INCLUDES_ABSOLUTE_platform_xcb ${SQT_INCLUDES_ABSOLUTE_platform_xcb} PARENT_SCOPE)
    set(SQT_DEFINES_platform_xcb
        "QT_USE_QSTRINGBUILDER"
        "QT_BUILDING_QT"
        "QT_NO_EXCEPTIONS"
        "QT_NO_DEBUG"
        "QT_NO_ACCESSIBILITY_ATSPI_BRIDGE"
        PARENT_SCOPE)
    set(SQT_LINKS_platform_xcb ${SQT_LINKS_platform_xcb} PARENT_SCOPE)
endfunction()

# CMake doesn't support properties that are specific to a single file and target
# simultaneously, so this will affect all files used in a CMakeLists.txt.
# CMake also doesn't really support lists of lists or datatypes because it's
# yet another garbage UNIX-style macro expansion language, so this auxiliary
# function thing is used instead of a loop.
function(sqt_set_file_specific_flags prefix)
    function(sqt_append_file_props files)
        list(TRANSFORM files PREPEND "${prefix}/")
        file(GLOB_RECURSE files ${files})
        set_source_files_properties(${files} PROPERTIES ${ARGN})
    endfunction()
    set(msvc_arch "")
    if("${SQT_COMPILER_MSVC_COMPATIBLE}")
        if("${SQT_TARGET_ARCH}" STREQUAL "sqt-arch-i386")
            set(msvc_arch COMPILE_FLAGS /arch:SSE2) # /arch:SSE2 is needed to enable __vectorcall on i386.
        endif()
    endif()
    sqt_append_file_props("*_ssse3.cpp"
       COMPILE_FLAGS $<$<CXX_COMPILER_ID:GNU,Clang>:-mssse3>
        ${msvc_arch})

    sqt_append_file_props("*_sse2.cpp"
        COMPILE_FLAGS $<$<CXX_COMPILER_ID:GNU,Clang>:-msse2>
        ${msvc_arch})

    sqt_append_file_props("*_sse4.cpp"
        COMPILE_FLAGS $<$<CXX_COMPILER_ID:GNU,Clang>:-msse4.1>
        COMPILE_DEFINITIONS $<$<CXX_COMPILER_ID:MSVC>:__SSE4_1__=1>
        ${msvc_arch})

    sqt_append_file_props("*_avx2.cpp"
        COMPILE_FLAGS $<$<CXX_COMPILER_ID:GNU,Clang>:-mavx2>
        ${msvc_arch})
endfunction()

macro(sqt_define_modules)
    def_info_bootstrap()
    def_info_corelib()
    def_info_gui()
    def_info_widgets()
    def_info_network()
    def_info_websockets()
    def_info_platform_windows()
    def_info_platform_xcb()
    sqt_adjust_file_lists("${SQT_CMAKE_DIR}/..")
    sqt_set_file_specific_flags("${SQT_CMAKE_DIR}/..")
endmacro()

macro(sqt_automoc_prerequisites)
    # CMake gained support for generating ninja depfiles using moc's depfile
    # support, when moc from Qt 5.15 or later is used. Unfortunately this
    # doesn't work well when moc itself is built earlier in the same project
    # (via an instance of `ExternalProject_Add`) with dependencies from
    # dependent targets to the bootstrap project via `add_dependencies. Work
    # around this by lying about the Qt version we're building.
    #
    # See also:
    # - https://gitlab.kitware.com/cmake/cmake/-/commit/aebfbcaa4650ec6f540cc53b96d44cdfb87d82a1
    # - https://gitlab.kitware.com/cmake/cmake/-/issues/18749
    set(Qt5Core_VERSION_MAJOR "5")
    set(Qt5Core_VERSION_MINOR "14")
endmacro()
