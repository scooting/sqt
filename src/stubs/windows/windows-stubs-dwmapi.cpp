// #define SQT_WINSTUBS_NO_PREPROC_FORWARDING

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
