include("sqt-init.cmake")

# 3.1: target_sources
# 3.5: GENERATOR_IS_MULTI_CONFIG
# 3.6: list(FILTER ...)
# 3.12: list(TRANSFORM ... PREPEND ...)
# 3.13: add_link_options
# 3.14: AUTOMOC_EXECUTABLE, AUTOUIC_EXECUTABLE, AUTORCC_EXECUTABLE
# 3.14: BUILD_RPATH_USE_ORIGIN
# 3.15: ADDITIONAL_CLEAN_FILES
# 3.15: Comma-separated matching for CXX_COMPILER_ID generator expressions
# 3.15: Target-specific control over MSVC runtime library with MSVC_RUNTIME_LIBRARY
cmake_minimum_required(VERSION 3.15)
work_around_wholly_unnecessary_cmake_convolution_before_project()
project("sqt-5.15.2" LANGUAGES CXX ASM ASM_MASM C)
work_around_wholly_unnecessary_cmake_convolution_after_project()

include(ExternalProject)
include("sqt.cmake")

set(sqt-cfg-bootstrap-toolchain-file "" CACHE FILEPATH "The path to a toolchain file for the bootstrap executables. Leave empty to use host compiler autodetection.")

if("${SQT_COMPILER_ID}" STREQUAL "sqt-compiler-id-msvc")
    add_compile_options("/MP")
endif()

sqt_detect_target()

if("${SQT_TARGET_OS}" STREQUAL "sqt-os-linux")
    find_package("Threads" REQUIRED)
endif()

function(sqt_add_bootstrap)
    set(bootstrap_toolchain "")
    set(passthrough_flags C_COMPILER CXX_COMPILER AR RANLIB LINKER EXE_LINKER_FLAGS SHARED_LINKER_FLAGS)
    set(cmake_C_COMPILER "")
    set(cmake_CXX_COMPILER "")
    set(cmake_AR "")
    set(cmake_RANLIB "")
    set(cmake_LINKER "")
    set(cmake_EXE_LINKER_FLAGS "")
    set(cmake_SHARED_LINKER_FLAGS "")
    if(sqt-cfg-bootstrap-toolchain-file)
        set(bootstrap_toolchain "-DCMAKE_TOOLCHAIN_FILE=${sqt-cfg-bootstrap-toolchain-file}")
    else()
        foreach(suffix ${passthrough_flags})
            if(sqt-bootstrap-CMAKE_${suffix})
                set(cmake_${suffix} "-DCMAKE_${suffix}=${sqt-bootstrap-CMAKE_${suffix}}")
            endif()
        endforeach()
    endif()
    ExternalProject_Add("bootstrap-executables"
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/bootstrap-executables"
        CMAKE_ARGS
            "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/bootstrap"
            "-DCMAKE_BUILD_TYPE=Release"
            "${cmake_C_COMPILER}"
            "${cmake_CXX_COMPILER}"
            "${cmake_AR}"
            "${cmake_RANLIB}"
            "${cmake_LINKER}"
            "${cmake_EXE_LINKER_FLAGS}"
            "${cmake_SHARED_LINKER_FLAGS}"
            "${bootstrap_toolchain}"
        INSTALL_DIR "${CMAKE_BINARY_DIR}/bootstrap")
endfunction()
sqt_add_bootstrap()

function(clean_bootstrap_executables)
    #ExternalProject_Get_Property("bootstrap-executables" STAMP_DIR)
    #set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${STAMP_DIR}")

    #ExternalProject_Get_Property("bootstrap-executables" LOG_DIR)
    #set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${LOG_DIR}")

    ExternalProject_Get_Property("bootstrap-executables" INSTALL_DIR)
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${INSTALL_DIR}")

    ExternalProject_Get_Property("bootstrap-executables" BINARY_DIR)
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${BINARY_DIR}")
endfunction()
clean_bootstrap_executables()

get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

function(sqt_target_set_outdir target)
    if("${GENERATOR_IS_MULTI_CONFIG}")
        set_target_properties("${target}"
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/$<CONFIG>
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/$<CONFIG>
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/$<CONFIG>)
    else()
        set_target_properties("${target}"
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out")
    endif()
endfunction()

function(sqt_plugin_set_outdir target kind)
    if("${GENERATOR_IS_MULTI_CONFIG}")
        set_target_properties("${target}"
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/$<CONFIG>/${kind}
            LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/$<CONFIG>/${kind}
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/$<CONFIG>/${kind})
    else()
        set_target_properties("${target}"
            PROPERTIES
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${kind}"
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${kind}"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${kind}")
    endif()
endfunction()

sqt_define_modules()
sqt_automoc_prerequisites()

function(def_msvcrt_support)
    if(MSVC)
        set(name "lib-msvcrt-support")
        sqt_glob_file_list(files
            "../src/msvcrt-support"
            "*.hpp"
            "*.cpp")
        add_library("${name}")
        target_sources("${name}" PRIVATE ${files})
        if("${SQT_TARGET_ARCH}" STREQUAL "sqt-arch-amd64")
            target_sources("${name}" PRIVATE "../src/msvcrt-support/msvcrt-support-chkstk-amd64.asm")
        endif()
        set_target_properties("${name}" PROPERTIES SQT_USE_MSVCRT TRUE)
        sqt_target_add_standard_flags("${name}")

        set(name "lib-msvcrt")
        add_library("${name}" STATIC IMPORTED)
        if("${SQT_TARGET_ARCH}" STREQUAL "sqt-arch-amd64")
            set_target_properties("${name}" PROPERTIES
                IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../src/msvcrt/amd64/msvcrt.lib")
        elseif("${SQT_TARGET_ARCH}" STREQUAL "sqt-arch-i386")
            set_target_properties("${name}" PROPERTIES
                IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../src/msvcrt/i386/msvcrt.lib")
        endif()
    endif()
endfunction()

function(sqt_link_msvcrt_support target)
    if("${SQT_COMPILER_MSVC_COMPATIBLE}")
        target_link_options("${target}" PRIVATE
            "/NODEFAULTLIB")
        target_link_libraries("${target}" PRIVATE
            "lib-msvcrt-support"
            "lib-msvcrt")
    endif()
endfunction()

function(def_pcre2)
    set(name "lib-pcre2")
    sqt_glob_file_list(files
        "../src/qt/qtbase/src/3rdparty/pcre2"
        "*.h"
        "src/pcre2_auto_possess.c"
        "src/pcre2_chartables.c"
        "src/pcre2_compile.c"
        "src/pcre2_config.c"
        "src/pcre2_context.c"
        "src/pcre2_dfa_match.c"
        "src/pcre2_error.c"
        "src/pcre2_extuni.c"
        "src/pcre2_find_bracket.c"
        "src/pcre2_jit_compile.c"
        "src/pcre2_maketables.c"
        "src/pcre2_match.c"
        "src/pcre2_match_data.c"
        "src/pcre2_newline.c"
        "src/pcre2_ord2utf.c"
        "src/pcre2_pattern_info.c"
        "src/pcre2_script_run.c"
        "src/pcre2_serialize.c"
        "src/pcre2_string_utils.c"
        "src/pcre2_study.c"
        "src/pcre2_substitute.c"
        "src/pcre2_substring.c"
        "src/pcre2_tables.c"
        "src/pcre2_ucd.c"
        "src/pcre2_valid_utf.c"
        "src/pcre2_xclass.c")
    add_library("${name}")
    target_sources("${name}" PRIVATE ${files})
    target_compile_definitions("${name}" PRIVATE
        "QT_NO_EXCEPTIONS"
        "QT_NO_DEBUG"
        "QT_BUILDING_QT"
        "HAVE_CONFIG_H"
        "PCRE2_CODE_UNIT_WIDTH=16"
        "PCRE2_STATIC")
    set_target_properties("${name}" PROPERTIES
        SQT_USE_MSVCRT TRUE
        POSITION_INDEPENDENT_CODE TRUE
        LINKER_LANGUAGE "C")
    sqt_target_add_standard_flags("${name}")
    sqt_target_set_outdir("${name}")
endfunction()

function(def_png)
    set(name "lib-png")
    sqt_glob_file_list(files
        "../src/qt/qtbase/src/3rdparty/libpng"
        "**.h"
        "png.c"
        "pngerror.c"
        "pngget.c"
        "pngmem.c"
        "pngpread.c"
        "pngread.c"
        "pngrio.c"
        "pngrtran.c"
        "pngrutil.c"
        "pngset.c"
        "pngtrans.c"
        "pngwio.c"
        "pngwrite.c"
        "pngwtran.c"
        "pngwutil.c")
    add_library("${name}")
    target_sources("${name}" PRIVATE ${files})
    target_include_directories("${name}" SYSTEM PRIVATE
        "../src/stubs/config-include"
        "../src/qt/qtbase/include"
        "../src/qt/qtbase/include/QtCore"
        "../src/qt/qtbase/src/3rdparty/zlib/src")
    target_compile_definitions("${name}" PRIVATE
        "QT_NO_EXCEPTIONS"
        "QT_NO_DEBUG"
        "QT_BUILDING_QT"
        "PNG_ARM_NEON_OPT=0"
        "PNG_POWERPC_VSX_OPT=0"
        "QT_BUILD_CORE_LIB=1"
        "QT_SHARED=1")
    set_target_properties("${name}" PROPERTIES
        SQT_USE_MSVCRT TRUE
        POSITION_INDEPENDENT_CODE TRUE
        LINKER_LANGUAGE "C")
    sqt_target_add_standard_flags("${name}")
    sqt_target_set_outdir("${name}")
endfunction()

function(def_harfbuzz)
    set(name "lib-harfbuzz")
    sqt_glob_file_list(files
       "../src/qt/qtbase/src/3rdparty/harfbuzz-ng"
        "src/*.h"
        "src/*.hh"
        "src/hb-blob.cc"
        "src/hb-buffer.cc"
        "src/hb-buffer-serialize.cc"
        "src/hb-common.cc"
        "src/hb-face.cc"
        "src/hb-font.cc"
        "src/hb-ot-tag.cc"
        "src/hb-set.cc"
        "src/hb-shape.cc"
        "src/hb-shape-plan.cc"
        "src/hb-shaper.cc"
        "src/hb-unicode.cc"
        "src/hb-warning.cc"

        "src/hb-ot-font.cc"
        "src/hb-ot-layout.cc"
        "src/hb-ot-map.cc"
        "src/hb-ot-math.cc"
        "src/hb-ot-shape.cc"
        "src/hb-ot-shape-complex-arabic.cc"
        "src/hb-ot-shape-complex-default.cc"
        "src/hb-ot-shape-complex-hangul.cc"
        "src/hb-ot-shape-complex-hebrew.cc"
        "src/hb-ot-shape-complex-indic.cc"
        "src/hb-ot-shape-complex-indic-table.cc"
        "src/hb-ot-shape-complex-myanmar.cc"
        "src/hb-ot-shape-complex-thai.cc"
        "src/hb-ot-shape-complex-tibetan.cc"
        "src/hb-ot-shape-complex-use.cc"
        "src/hb-ot-shape-complex-use-table.cc"
        "src/hb-ot-shape-fallback.cc"
        "src/hb-ot-shape-normalize.cc"
        "src/hb-ot-var.cc")
    add_library("${name}")
    target_sources("${name}" PRIVATE ${files})
    target_include_directories("${name}" SYSTEM PRIVATE
        "../src/stubs/config-include"
        "../src/qt/qtbase/include"
        "../src/qt/qtbase/src/3rdparty/harfbuzz-ng/include"
        "../src/qt/qtbase/src/3rdparty/harfbuzz-ng")
    target_compile_definitions("${name}" PRIVATE
        "QT_NO_EXCEPTIONS"
        "QT_NO_DEBUG"
        "QT_BUILDING_QT"
        "QT_NO_VERSION_TAGGING"
        "HAVE_CONFIG_H"
        "HAVE_OT"
        "HB_DISABLE_DEPRECATED"
        "HB_EXTERN="
        "HB_NO_UNICODE_FUNCS"
        $<$<PLATFORM_ID:Windows>:HB_NO_WIN1256>)
    set_target_properties("${name}" PROPERTIES
        SQT_USE_MSVCRT TRUE
        POSITION_INDEPENDENT_CODE TRUE
        LINKER_LANGUAGE "C")
    sqt_target_add_standard_flags("${name}")
    sqt_target_set_outdir("${name}")
endfunction()

function(def_qtcore)
    set(name "Qt5Core")
    add_library("${name}" SHARED)
    target_sources("${name}" PRIVATE
        ${SQT_FILES_corelib_generic}
        ${SQT_FILES_corelib_${SQT_files_tag}})
    target_link_libraries("${name}" PRIVATE ${SQT_LINKS_corelib})
    target_include_directories("${name}" SYSTEM PRIVATE ${SQT_INCLUDES_corelib})
    target_compile_definitions("${name}" PRIVATE ${SQT_DEFINES_corelib})
    set_target_properties("${name}" PROPERTIES PREFIX "")
    sqt_add_platform_definitions("${name}")
    sqt_link_msvcrt_support("${name}")
    set_target_properties("${name}" PROPERTIES
        SQT_USE_MSVCRT TRUE
        POSITION_INDEPENDENT_CODE TRUE
        LINKER_LANGUAGE "C")
    sqt_target_add_standard_flags("${name}")
    sqt_use_moc("${name}")
    sqt_target_set_outdir("${name}")
endfunction()

function(def_qtgui)
    set(name "Qt5Gui")
    add_library("${name}" SHARED)
    target_sources("${name}" PRIVATE
        ${SQT_FILES_gui_generic}
        ${SQT_FILES_gui_${SQT_files_tag}})
    target_link_libraries("${name}" PRIVATE "Qt5Core" ${SQT_LINKS_gui})
    target_include_directories("${name}" SYSTEM PRIVATE ${SQT_INCLUDES_gui})
    target_compile_definitions("${name}" PRIVATE ${SQT_DEFINES_gui})
    set_target_properties("${name}" PROPERTIES PREFIX "")
    sqt_add_platform_definitions("${name}")
    sqt_link_msvcrt_support("${name}")
    set_target_properties("${name}" PROPERTIES
        SQT_USE_MSVCRT TRUE
        POSITION_INDEPENDENT_CODE TRUE
        LINKER_LANGUAGE "C")
    sqt_target_add_standard_flags("${name}")
    sqt_use_moc("${name}")
    sqt_target_set_outdir("${name}")
endfunction()

function(def_qtwidgets)
    set(name "Qt5Widgets")
    add_library("${name}" SHARED)
    target_sources("${name}" PRIVATE
        ${SQT_FILES_widgets_generic}
        ${SQT_FILES_widgets_${SQT_files_tag}})
    target_link_libraries("${name}" PRIVATE "Qt5Core" "Qt5Gui" ${SQT_LINKS_widgets})
    target_include_directories("${name}" SYSTEM PRIVATE ${SQT_INCLUDES_widgets})
    target_compile_definitions("${name}" PRIVATE ${SQT_DEFINES_widgets})
    set_target_properties("${name}" PROPERTIES PREFIX "")
    sqt_add_platform_definitions("${name}")
    sqt_link_msvcrt_support("${name}")
    set_target_properties("${name}" PROPERTIES
        SQT_USE_MSVCRT TRUE
        POSITION_INDEPENDENT_CODE TRUE
        LINKER_LANGUAGE "C")
    sqt_target_add_standard_flags("${name}")
    sqt_use_moc("${name}")
    sqt_target_set_outdir("${name}")
endfunction()

function(def_qtnetwork)
    set(name "Qt5Network")
    add_library("${name}" SHARED)
    target_sources("${name}" PRIVATE
        ${SQT_FILES_network_generic}
        ${SQT_FILES_network_win32})
    target_link_libraries("${name}" PRIVATE "Qt5Core" ${SQT_LINKS_network})
    target_include_directories("${name}" SYSTEM PRIVATE ${SQT_INCLUDES_network})
    target_compile_definitions("${name}" PRIVATE ${SQT_DEFINES_network})
    set_target_properties("${name}" PROPERTIES PREFIX "")
    sqt_add_platform_definitions("${name}")
    sqt_link_msvcrt_support("${name}")
    set_target_properties("${name}" PROPERTIES SQT_USE_MSVCRT TRUE)
    sqt_target_add_standard_flags("${name}")
    sqt_use_moc("${name}")
    sqt_target_set_outdir("${name}")
endfunction()

function(def_qtwebsockets)
    set(name "Qt5WebSockets")
    add_library("${name}" SHARED)
    target_sources("${name}" PRIVATE
        ${SQT_FILES_websockets_generic}
        ${SQT_FILES_websockets_win32})
    target_link_libraries("${name}" PRIVATE "Qt5Core" "Qt5Network" ${SQT_LINKS_websockets})
    target_include_directories("${name}" SYSTEM PRIVATE ${SQT_INCLUDES_websockets})
    target_compile_definitions("${name}" PRIVATE ${SQT_DEFINES_websockets})
    set_target_properties("${name}" PROPERTIES PREFIX "")
    sqt_add_platform_definitions("${name}")
    sqt_link_msvcrt_support("${name}")
    set_target_properties("${name}" PROPERTIES SQT_USE_MSVCRT TRUE)
    sqt_target_add_standard_flags("${name}")
    sqt_use_moc("${name}")
    sqt_target_set_outdir("${name}")
endfunction()

function(def_platform_windows)
    set(name "qwindows")
    add_library("${name}" SHARED)
    target_sources("${name}" PRIVATE
        ${SQT_FILES_platform_windows_generic}
        ${SQT_FILES_platform_windows_${SQT_files_tag}})
    target_link_libraries("${name}" PRIVATE "Qt5Gui" "Qt5Core" ${SQT_LINKS_platform_windows})
    target_include_directories("${name}" SYSTEM PRIVATE ${SQT_INCLUDES_platform_windows})
    target_compile_definitions("${name}" PRIVATE ${SQT_DEFINES_platform_windows})
    set_target_properties("${name}" PROPERTIES PREFIX "")
    sqt_add_platform_definitions("${name}")
    sqt_link_msvcrt_support("${name}")
    set_target_properties("${name}" PROPERTIES
        SQT_USE_MSVCRT TRUE
        POSITION_INDEPENDENT_CODE TRUE
        LINKER_LANGUAGE "C")
    sqt_target_add_standard_flags("${name}")
    sqt_use_moc("${name}")
    sqt_plugin_set_outdir("${name}" "platforms")
endfunction()

function(def_platform_xcb)
    set(name "qxcb")
    add_library("${name}" SHARED)
    target_sources("${name}" PRIVATE
        ${SQT_FILES_platform_xcb_generic}
        ${SQT_FILES_platform_xcb_${SQT_files_tag}})
    target_link_libraries("${name}" PRIVATE "Qt5Gui" "Qt5Core" ${SQT_LINKS_platform_xcb})
    target_include_directories("${name}" SYSTEM PRIVATE ${SQT_INCLUDES_platform_xcb})
    target_compile_definitions("${name}" PRIVATE ${SQT_DEFINES_platform_xcb})
    set_target_properties("${name}" PROPERTIES PREFIX "")
    sqt_add_platform_definitions("${name}")
    sqt_link_msvcrt_support("${name}")
    set_target_properties("${name}" PROPERTIES
        SQT_USE_MSVCRT TRUE
        POSITION_INDEPENDENT_CODE TRUE
        LINKER_LANGUAGE "C")
    sqt_target_add_standard_flags("${name}")
    sqt_use_moc("${name}")
    sqt_plugin_set_outdir("${name}" "platforms")
endfunction()

function(def_qt)
    set(name "qt5")
    add_library("${name}" SHARED)
    target_sources("${name}" PRIVATE
        ${SQT_FILES_corelib_generic}
        ${SQT_FILES_corelib_${SQT_files_tag}}
        ${SQT_FILES_gui_generic}
        ${SQT_FILES_gui_${SQT_files_tag}}
        ${SQT_FILES_widgets_generic}
        ${SQT_FILES_widgets_${SQT_files_tag}})
    target_link_libraries("${name}" PRIVATE
        ${SQT_LINKS_corelib}
        ${SQT_LINKS_gui}
        ${SQT_LINKS_widgets})
    target_include_directories("${name}" SYSTEM PRIVATE
        ${SQT_INCLUDES_corelib}
        ${SQT_INCLUDES_gui}
        ${SQT_INCLUDES_widgets})
    target_compile_definitions("${name}" PRIVATE
        ${SQT_DEFINES_corelib}
        ${SQT_DEFINES_gui}
        ${SQT_DEFINES_widgets}
        "QT_STATICPLUGIN")
    if("${SQT_TARGET_OS}" STREQUAL "sqt-os-windows")
        target_sources("${name}" PRIVATE
            ${SQT_FILES_platform_windows_generic}
            ${SQT_FILES_platform_windows_${SQT_files_tag}}
            "../src/stubs/qwindows/qwindows-bundle-main.cpp")
        target_link_libraries("${name}" PRIVATE
            ${SQT_LINKS_platform_windows})
        target_include_directories("${name}" SYSTEM PRIVATE
            ${SQT_INCLUDES_platform_windows})
        target_compile_definitions("${name}" PRIVATE
            ${SQT_DEFINES_platform_windows})
    elseif("${SQT_TARGET_OS}" STREQUAL "sqt-os-linux")
        target_sources("${name}" PRIVATE
            ${SQT_FILES_platform_xcb_generic}
            ${SQT_FILES_platform_xcb_${SQT_files_tag}}
            "../src/stubs/qxcb/qxcb-bundle-main.cpp")
        target_link_libraries("${name}" PRIVATE
            ${SQT_LINKS_platform_xcb})
        target_include_directories("${name}" SYSTEM PRIVATE
            ${SQT_INCLUDES_platform_xcb})
        target_compile_definitions("${name}" PRIVATE
            ${SQT_DEFINES_platform_xcb})
    endif()
    set_target_properties("${name}" PROPERTIES PREFIX "")
    sqt_add_platform_definitions("${name}")
    sqt_link_msvcrt_support("${name}")
    set_target_properties("${name}" PROPERTIES
        SQT_USE_MSVCRT TRUE
        POSITION_INDEPENDENT_CODE TRUE
        LINKER_LANGUAGE "C")
    sqt_target_add_standard_flags("${name}")
    sqt_use_moc("${name}")
    sqt_target_set_outdir("${name}")
endfunction()

function(def_testapp)
    set(name "testapp")
    add_executable("${name}" WIN32)
    target_sources("${name}" PRIVATE
        "../src/testapp/main-testapp.cpp")
    target_include_directories("${name}" SYSTEM PRIVATE
        "../src/qt/qtbase/include"
        "../src/qt/qtbase/include/QtCore"
        "../src/qt/qtbase/include/QtGui"
        "../src/qt/qtbase/include/QtWidgets")
    if(TRUE)
        target_link_libraries("${name}" PRIVATE
            "qt5")
    else()
        target_link_libraries("${name}" PRIVATE
            "Qt5Core"
            "Qt5Gui"
            "Qt5Widgets")
        if("${SQT_TARGET_OS}" STREQUAL "sqt-os-windows")
            add_dependencies("${name}" "qwindows")
        elseif("${SQT_TARGET_OS}" STREQUAL "sqt-os-linux")
            add_dependencies("${name}" "qxcb")
        endif()
    endif()
    sqt_link_msvcrt_support("${name}")
    set_target_properties("${name}" PROPERTIES
        SQT_USE_MSVCRT TRUE
        LINKER_LANGUAGE "C")
    sqt_target_add_standard_flags("${name}")
    sqt_target_set_outdir("${name}")
    if("${SQT_TARGET_OS}" STREQUAL "sqt-os-linux")
        target_link_libraries("${name}" PRIVATE ${CMAKE_DL_LIBS} "GL")
        target_link_options("${name}" PRIVATE "-pthread" "-Wl,--disable-new-dtags")
        set_target_properties("${name}" PROPERTIES BUILD_RPATH_USE_ORIGIN TRUE)
    endif()
endfunction()

def_pcre2()
def_png()
def_harfbuzz()
def_msvcrt_support()

def_platform_windows()
def_platform_xcb()

def_qt()
def_qtcore()
def_qtgui()
def_qtwidgets()
def_qtnetwork()
def_qtwebsockets()

def_testapp()
