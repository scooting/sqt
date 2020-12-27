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

function(sqt_append_mkspecs includes)
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
        list(APPEND ${includes} "src/qt/qtbase/mkspecs/win32-g++")
    elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
        list(APPEND ${includes} "src/qt/qtbase/mkspecs/win32-clang-g++")
    elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC")
        list(APPEND ${includes} "src/qt/qtbase/mkspecs/win32-msvc")
    endif()
    set(${includes} ${${includes}} PARENT_SCOPE)
endfunction()

function(sqt_append_private_include includes qt_module_name)
    list(APPEND ${includes}
        "src/qt/qtbase/include/${qt_module_name}"
        "src/qt/qtbase/include/${qt_module_name}/5.15.2"
        "src/qt/qtbase/include/${qt_module_name}/5.15.2/${qt_module_name}"
        "src/qt/qtbase/include/${qt_module_name}/5.15.2/${qt_module_name}/private")
    set(${includes} ${${includes}} PARENT_SCOPE)
endfunction()

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
            list(TRANSFORM xs PREPEND "${prefix}/")
            set(SQT_FILES_${module}_${os} ${xs} PARENT_SCOPE)
        endforeach()

        set(xs ${SQT_INCLUDES_${module}})
        list(TRANSFORM xs PREPEND "${prefix}/")
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
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:forScope>
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:rvalueCast>
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:wchar_t>
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:strictStrings>
            # $<$<COMPILE_LANGUAGE:CXX,C>:/bigobj>
            $<$<COMPILE_LANGUAGE:CXX,C>:/wd4996> # Disables deprecation warnings. QLinkedList is deprecated but is used anyway.
            $<$<COMPILE_LANGUAGE:CXX,C>:/wd4577> # 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc
            )
        # 1920-1929 = VS 2019
        # 1910-1919 = VS 2017
        # 1900 = VS 2015
        if(MSVC_VERSION GREATER_EQUAL "1910")
            target_compile_options("${target}" PRIVATE
                $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:ternary>
                $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:__cplusplus>
                )
        endif()
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
        target_compile_options(
            "${target}" PRIVATE
            $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
            $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
            $<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>
            $<$<COMPILE_LANGUAGE:CXX,C>:-fdata-sections>
            $<$<COMPILE_LANGUAGE:CXX,C>:-ffunction-sections>
            $<$<COMPILE_LANGUAGE:CXX,C>:-fvisibility=hidden>
            $<$<COMPILE_LANGUAGE:CXX,C>:-Wno-deprecated-declarations> # QLinkedList is deprecated but is used anyway.
            )
    endif()

    if("${SQT_COMPILER_ID}" STREQUAL "sqt-compiler-id-msvc")
        target_compile_options("${target}" PRIVATE
            $<$<COMPILE_LANGUAGE:CXX,C>:/Zc:referenceBinding>)
    endif()

    target_compile_definitions("${target}" PRIVATE "QT_NO_EXCEPTIONS")

    get_target_property(target_type "${target}" TYPE)
    if(("${target_type}" STREQUAL "EXECUTABLE") OR ("${target_type}" STREQUAL "SHARED_LIBRARY"))
        if(SQT_COMPILER_MSVC_COMPATIBLE)
            target_compile_options("${target}" PRIVATE
                $<$<COMPILE_LANGUAGE:CXX,C>:/Zl>)
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
            if("${SQT_TARGET_ARCH}" STREQUAL "sqt-arch-i386")
                target_link_options("${target}" PRIVATE
                    "-static-libgcc" # Qt uses 64-bit multiplication and division on 32-bit machines. Statically link libgcc here instead of rebuilding the support functions from scratch.
                    )
            endif()
            if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
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
        AUTOMOC_EXECUTABLE "${CMAKE_BINARY_DIR}/bootstrap/moc${CMAKE_EXECUTABLE_SUFFIX}"
        AUTOUIC_EXECUTABLE "${CMAKE_BINARY_DIR}/bootstrap/uic${CMAKE_EXECUTABLE_SUFFIX}"
        AUTORCC_EXECUTABLE "${CMAKE_BINARY_DIR}/bootstrap/rcc${CMAKE_EXECUTABLE_SUFFIX}")
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
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        list(APPEND SQT_LINKS_corelib "ws2_32" "version" "netapi32" "userenv" "winmm" "mpr")
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
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        list(APPEND SQT_LINKS_gui "opengl32")
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
    def_info_platform_windows()
    sqt_adjust_file_lists("${SQT_CMAKE_DIR}/..")
    sqt_set_file_specific_flags("${SQT_CMAKE_DIR}/..")
endmacro()

macro(sqt_automoc_prerequisites)
    set(Qt5Core_VERSION_MAJOR "5")
    set(Qt5Core_VERSION_MINOR "15")
endmacro()
