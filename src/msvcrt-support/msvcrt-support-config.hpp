/*
Copyright (c) 2018-2021 Imran Hameed
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#define MSVCRT_SUPPORT_ENABLE_THREAD_SAFE_STATICS 0
#define MSVCRT_SUPPORT_ENABLE_LOCKIT 1
#define MSVCRT_SUPPORT_ENABLE_CRT_DEBUG 1
#define MSVCRT_SUPPORT_ENABLE_C_STDIO 1
#define MSVCRT_SUPPORT_ENABLE_POSIX_IO 1

// Some UCRT symbols have explicit support for replacement by static in-module
// stubs. This preprocessor definition disables the "dllimport" directives on
// these symbols, so that they will be found in this static glue library.
#if defined(_ACRTIMP_ALT)
    #undef _ACRTIMP_ALT
#endif
#define _ACRTIMP_ALT

// Exceptions are not supported by this library.
#if defined(_HAS_EXCEPTIONS)
    #undef _HAS_EXCEPTIONS
#endif
#define _HAS_EXCEPTIONS 0

// The MSVC2015 headers, by default, delegate to internal UCRT symbols with
// inline definitions for legacy functions (e.g. `sprintf`, `scanf`, etc.). But
// these headers can be configured to not do this by defining this preprocessor
// flag, which then allows each of these stdio functions to be handled
// individually, either by delegating their implementation to msvcrt.dll
// directly, or by handling them internally in this static glue library.
#if defined(_NO_CRT_STDIO_INLINE)
    #undef _NO_CRT_STDIO_INLINE
#endif
#define _NO_CRT_STDIO_INLINE 1

// FILE * is defined by this library. The layout of FILE * in msvcrt.dll differs
// from that in the UCRT.
#if defined(_FILE_DEFINED)
    #undef _FILE_DEFINED
#endif
#define _FILE_DEFINED 1
