#include "windows-stubs.hpp"
#include "windows-stubs-util.hpp"

#include "wincrypt.h"

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_advapi32();
} }

#define SQT_WINSTUBS_IMPLEMENTATION_advapi32
#define SQT_WINSTUBS_NO_FPTR_DECL
#include "windows-stubs-gen.hpp"
#undef SQT_WINSTUBS_NO_FPTR_DECL

namespace sqt { namespace winapi {
    static BOOLEAN WINAPI
    stub_SystemFunction036(PVOID rnd, ULONG len) {
        auto crypt = HCRYPTPROV { };
        auto init = false;
        BOOL ret = FALSE;
        {
            auto const success = CryptAcquireContextW(
                &crypt, nullptr, L"Microsoft Base Cryptographic Provider v1.0",
                PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
            if (!success) goto cleanup;
            init = true;
        }
        {
            auto const success = CryptGenRandom(crypt, len, static_cast<BYTE *>(rnd));
            if (!success) goto cleanup;
            ret = TRUE;
        }
        cleanup:
        if (crypt || init) {
            CryptReleaseContext(crypt, 0);
        }
        return ret;
    }
} }

namespace sqt { namespace winapi {

    static void Q_NEVER_INLINE
    winapi_resolve_advapi32() {
        winapi_resolve(L"System32\\advapi32.dll",
            zz_funcs_advapi32.ptrs, zz_stub_funcs_advapi32.ptrs,
            &symnames_advapi32[0][0], stride(symnames_advapi32), count(symnames_advapi32));
    }

} }
