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

#include "windows-stubs.hpp"
#include "windows-stubs-util.hpp"

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_dwmapi();
} }

#define SQT_WINSTUBS_IMPLEMENTATION_dwmapi
#define SQT_WINSTUBS_NO_FPTR_DECL
#include "windows-stubs-gen.hpp"
#undef SQT_WINSTUBS_NO_FPTR_DECL

namespace sqt { namespace winapi {
    static ::HRESULT __stdcall
    stub_DwmIsCompositionEnabled(BOOL *pfEnabled) {
        *pfEnabled = FALSE;
        return S_OK;
    }

    static ::HRESULT __stdcall
    stub_DwmEnableBlurBehindWindow(HWND, DWM_BLURBEHIND const *) {
        return S_OK;
    }

    static ::HRESULT __stdcall
    stub_DwmSetWindowAttribute(
        HWND hwnd,
        DWORD dwAttribute,
        LPCVOID pvAttribute,
        DWORD cbAttribute)
    {
        return S_OK;
    }

    static ::HRESULT __stdcall
    stub_DwmGetWindowAttribute(
        HWND hwnd,
        DWORD dwAttribute,
        PVOID pvAttribute,
        DWORD cbAttribute)
    {
        return S_OK;
    }
} }

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_dwmapi() {
        winapi_resolve(L"System32\\dwmapi.dll",
            zz_funcs_dwmapi.ptrs, zz_stub_funcs_dwmapi.ptrs,
            &symnames_dwmapi[0][0], stride(symnames_dwmapi), count(symnames_dwmapi));
    }
} }
