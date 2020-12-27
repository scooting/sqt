/*
Copyright (c) 2020-2021 Imran Hameed
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

#include <initguid.h>
// #include <shlguid.h>
// #include <wincrypt.h>

#include "windows-stubs.hpp"
#include "windows-stubs-util.hpp"

#include <atomic>

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_gdi32();
} }

#define SQT_WINSTUBS_IMPLEMENTATION_gdi32
#define SQT_WINSTUBS_NO_FPTR_DECL
#include "windows-stubs-gen.hpp"
#undef SQT_WINSTUBS_NO_FPTR_DECL

namespace sqt { namespace winapi {
    static int __stdcall
    stub_AddFontResourceExW(
        LPCTSTR lpszFilename,
        DWORD fl,
        PVOID pdv)
    {
        return AddFontResourceW(lpszFilename);
    }

    static HANDLE __stdcall
    stub_AddFontMemResourceEx(
        PVOID pbFont,
        DWORD cbFont,
        PVOID pdv,
        DWORD *pcFonts)
    {
        return 0;
    }

    static BOOL __stdcall
    stub_RemoveFontResourceExW(
        LPCTSTR lpFileName,
        DWORD fl,
        PVOID pdv)
    {
        return RemoveFontResourceW(lpFileName);
    }

    static BOOL __stdcall
    stub_RemoveFontMemResourceEx(
        HANDLE fh)
    {
        return 0;
    }

    static DWORD __stdcall
    stub_SetLayout(
        HDC hdc,
        DWORD dwLayout)
    {
        return 0;
    }
} }

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_gdi32() {
        winapi_resolve(L"System32\\gdi32.dll",
            zz_funcs_gdi32.ptrs, zz_stub_funcs_gdi32.ptrs,
            &symnames_gdi32[0][0], stride(symnames_gdi32), count(symnames_gdi32));
    }
} }
