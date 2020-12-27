// #define SQT_WINSTUBS_NO_PREPROC_FORWARDING

#include "windows-stubs.hpp"
#include "windows-stubs-util.hpp"

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_imm32();
} }

#define SQT_WINSTUBS_IMPLEMENTATION_imm32
#define SQT_WINSTUBS_NO_FPTR_DECL
#include "windows-stubs-gen.hpp"
#undef SQT_WINSTUBS_NO_FPTR_DECL

namespace sqt { namespace winapi {
    static BOOL __stdcall
    stub_ImmAssociateContextEx(
        HWND hWnd,
        HIMC hIMC,
        DWORD dwFlags)
    {
        // XXXih: ???
        ImmAssociateContext(hWnd, hIMC);
        return TRUE;
    }
} }

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_imm32() {
        winapi_resolve(L"System32\\imm32.dll",
            zz_funcs_imm32.ptrs, zz_stub_funcs_imm32.ptrs,
            &symnames_imm32[0][0], stride(symnames_imm32), count(symnames_imm32));
    }
} }
