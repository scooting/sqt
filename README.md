CMake and CLion Option Cheat Sheet
==================================

Use Ninja instead of Make or NMake:

    -G Ninja

Use LLD (and LLVM AR) instead of GNU LD, which significantly improves linking
time:

    -DCMAKE_EXE_LINKER_FLAGS="--ld-path=/programs/llvm/13.x/bin/ld.lld"
    -DCMAKE_SHARED_LINKER_FLAGS="--ld-path=/programs/llvm/13.x/bin/ld.lld"
    -DCMAKE_AR="/programs/llvm/13.x/bin/llvm-ar"
    -DCMAKE_RANLIB="/programs/llvm/13.x/bin/llvm-ranlib"

On Windows:

    -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld
    -DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=lld
    -DCMAKE_AR="C:\path\to\mingw\bin\llvm-ar.exe"
    -DCMAKE_RANLIB="C:\path\to\mingw\bin\llvm-ranlib.exe"

Enable COMDAT folding/identical code folding with LLD:

    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=lld -Wl,--icf=all"
    -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=lld -Wl,--icf=all"

Use Clang ThinLTO:

    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=lld"
    -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=lld"
    -DCMAKE_C_FLAGS="-flto=thin"
    -DCMAKE_CXX_FLAGS=-flto=thin"
    -DCMAKE_AR="C:\path\to\mingw\bin\llvm-ar.exe"
    -DCMAKE_RANLIB="C:\path\to\mingw\bin\llvm-ranlib.exe"

Use MSVC link.exe instead of lld-link.exe when using clang-cl:

    -DCMAKE_LINKER="C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.16.27023/bin/HostX64/x64/link.exe"

Why? lld-link.exe's implementation of /OPT:REF is extremely aggressive and can
incorrectly eliminate live symbols.

GNU Toolchain Notes
===================

From within CMake: split debug information into a separate file, remove all
non-essential crud from the output binary, and link the output binary to the
debug information file:

    COMMAND ${CMAKE_OBJCOPY} --only-keep-debug path/to/target${CMAKE_EXECUTABLE_SUFFIX} path/to/target${CMAKE_EXECUTABLE_SUFFIX}.dbg
    COMMAND ${CMAKE_OBJCOPY} --strip-all path/to/target${CMAKE_EXECUTABLE_SUFFIX}
    COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=path/to/target${CMAKE_EXECUTABLE_SUFFIX}.dbg path/to/target${CMAKE_EXECUTABLE_SUFFIX}

You may replace the second line with:

    COMMAND ${CMAKE_OBJCOPY} --strip-debug path/to/target${CMAKE_EXECUTABLE_SUFFIX}

if you want to strip the output binary more conservatively.

MinGW Notes
===========

You may want to try Dr. Mingw for just-in-time debugging. Note that this will
mostly take over all just-in-time debugging.

https://github.com/jrfonseca/drmingw
