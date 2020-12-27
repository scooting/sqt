include("../sqt-init.cmake")

cmake_minimum_required(VERSION 3.15)
work_around_wholly_unnecessary_cmake_convolution_before_project()
project("bootstrap-executables" LANGUAGES CXX ASM ASM_MASM C)
work_around_wholly_unnecessary_cmake_convolution_after_project()

include("../sqt.cmake")

if("${SQT_COMPILER_ID}" STREQUAL "sqt-compiler-id-msvc")
    add_compile_options("/MP")
endif()

sqt_detect_target()
sqt_define_modules()

macro(statically_link_gcc_runtime_libraries)
    if("${SQT_COMPILER_GCC_COMPATIBLE}")
        add_link_options("-static-libgcc" "-static-libstdc++")
    endif()
endmacro()

function(def_bootstrap)
    add_library("lib-qt-bootstrap" ${SQT_FILES_bootstrap_generic} ${SQT_FILES_bootstrap_${SQT_files_tag}})
    target_include_directories("lib-qt-bootstrap" SYSTEM PRIVATE ${SQT_INCLUDES_bootstrap})
    sqt_add_bootstrap_definitions("lib-qt-bootstrap")
    sqt_add_platform_definitions("lib-qt-bootstrap")
    sqt_target_add_standard_flags("lib-qt-bootstrap")
endfunction()

function(def_moc)
    file(GLOB_RECURSE files
        "../../src/qt/qtbase/src/tools/moc/*.h")
    list(APPEND files
        "../../src/qt/qtbase/src/tools/moc/main.cpp"
        "../../src/qt/qtbase/src/tools/moc/moc.cpp"
        "../../src/qt/qtbase/src/tools/moc/collectjson.cpp"
        "../../src/qt/qtbase/src/tools/moc/preprocessor.cpp"
        "../../src/qt/qtbase/src/tools/moc/generator.cpp"
        "../../src/qt/qtbase/src/tools/moc/parser.cpp"
        "../../src/qt/qtbase/src/tools/moc/token.cpp")
    add_executable("moc" ${files})
    target_include_directories("moc" SYSTEM PRIVATE ${SQT_INCLUDES_bootstrap})
    target_compile_definitions("moc" PRIVATE "QT_MOC")
    sqt_add_bootstrap_definitions("moc")
    sqt_add_platform_definitions("moc")
    target_link_libraries("moc"
        "lib-qt-bootstrap"
        $<$<PLATFORM_ID:Windows>:netapi32>)
    set_target_properties("moc" PROPERTIES sqt-target-prop-dont-strip-versioned-symbols true)
    sqt_target_add_standard_flags("moc")
endfunction()

function(def_uic)
    file(GLOB_RECURSE files
        "../../src/qt/qtbase/src/tools/uic/*.h"
        "../../src/qt/qtbase/src/tools/uic/*.cpp")
    list(APPEND files
        "../../src/qt/qtbase/src/tools/uic/main.cpp"
        "../../src/qt/qtbase/src/tools/uic/uic.cpp"
        "../../src/qt/qtbase/src/tools/uic/customwidgetsinfo.cpp"
        "../../src/qt/qtbase/src/tools/uic/databaseinfo.cpp"
        "../../src/qt/qtbase/src/tools/uic/driver.cpp"
        "../../src/qt/qtbase/src/tools/uic/treewalker.cpp"
        "../../src/qt/qtbase/src/tools/uic/ui4.cpp"
        "../../src/qt/qtbase/src/tools/uic/validator.cpp")
    add_executable("uic" ${files})
    target_include_directories("uic" SYSTEM PRIVATE
        ${SQT_INCLUDES_bootstrap}
        "../../src/qt/qtbase/src/tools/uic"
        "../../src/qt/qtbase/src/tools/uic/cpp"
        "../../src/qt/qtbase/src/tools/uic/python"
        "../../src/qt/qtbase/src/tools/uic/shared")
    target_compile_definitions("uic" PRIVATE "QT_UIC")
    sqt_add_bootstrap_definitions("uic")
    sqt_add_platform_definitions("uic")
    target_link_libraries("uic"
        "lib-qt-bootstrap"
        $<$<PLATFORM_ID:Windows>:netapi32>)
    set_target_properties("uic" PROPERTIES sqt-target-prop-dont-strip-versioned-symbols true)
    sqt_target_add_standard_flags("uic")
endfunction()

function(def_rcc)
    add_executable("rcc"
        "../../src/qt/qtbase/src/tools/rcc/main.cpp"
        "../../src/qt/qtbase/src/tools/rcc/rcc.cpp"
        "../../src/qt/qtbase/src/tools/rcc/rcc.h")
    target_include_directories("rcc" SYSTEM PRIVATE
        ${SQT_INCLUDES_bootstrap}
        "../../src/qt/qtbase/src/tools/rcc")
    target_compile_definitions("rcc" PRIVATE "QT_RCC")
    sqt_add_bootstrap_definitions("rcc")
    sqt_add_platform_definitions("rcc")
    target_link_libraries("rcc"
        "lib-qt-bootstrap"
        $<$<PLATFORM_ID:Windows>:netapi32>)
    set_target_properties("rcc" PROPERTIES sqt-target-prop-dont-strip-versioned-symbols true)
    sqt_target_add_standard_flags("rcc")
endfunction()

statically_link_gcc_runtime_libraries()
def_bootstrap()
def_moc()
def_uic()
def_rcc()

add_executable("strip-versioned-symbols" "../../src/strip-versioned-symbols/strip-versioned-symbols.cpp")
set_target_properties("strip-versioned-symbols" PROPERTIES sqt-target-prop-dont-strip-versioned-symbols true)
set_target_properties("strip-versioned-symbols" PROPERTIES sqt-target-prop-msvc-keep-runtime-library true)
if(MSVC)
    target_compile_options("strip-versioned-symbols" PRIVATE "/MD")
endif()
sqt_target_add_standard_flags("strip-versioned-symbols")

install(TARGETS "strip-versioned-symbols" DESTINATION ${CMAKE_INSTALL_PREFIX})
install(TARGETS "moc" DESTINATION ${CMAKE_INSTALL_PREFIX})
install(TARGETS "uic" DESTINATION ${CMAKE_INSTALL_PREFIX})
install(TARGETS "rcc" DESTINATION ${CMAKE_INSTALL_PREFIX})
