// #define SQT_WINSTUBS_NO_PREPROC_FORWARDING

#include "windows-stubs.hpp"
#include "windows-stubs-util.hpp"

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_wtsapi32();
} }

#define SQT_WINSTUBS_IMPLEMENTATION_wtsapi32
#define SQT_WINSTUBS_NO_FPTR_DECL
#include "windows-stubs-gen.hpp"
#undef SQT_WINSTUBS_NO_FPTR_DECL

namespace sqt { namespace winapi {
    static BOOL __stdcall
    stub_WTSQuerySessionInformationW(
        HANDLE,
        DWORD,
        WTS_INFO_CLASS,
        LPWSTR *,
        DWORD *)
    {
        return FALSE;
    }

    static void __stdcall
    stub_WTSFreeMemory(PVOID ptr) { }
} }

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_wtsapi32() {
        winapi_resolve(L"System32\\wtsapi32.dll",
            zz_funcs_wtsapi32.ptrs, zz_stub_funcs_wtsapi32.ptrs,
            &symnames_wtsapi32[0][0], stride(symnames_wtsapi32), count(symnames_wtsapi32));
    }
} }
