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
#if defined(_MSC_VER)

#if defined(_ACRTIMP)
    #undef _ACRTIMP
#endif

#if defined(_ACRTIMP_ALT)
    #undef _ACRTIMP_ALT
#endif

#define _ACRTIMP
#define _ACRTIMP_ALT

#include "msvcrt-support-stubs-dirty.hpp"

#include "msvcrt-support-config.hpp"

extern "C" {
    struct _iobuf;
    using FILE = _iobuf;
}

#if MSVCRT_SUPPORT_ENABLE_THREAD_SAFE_STATICS
    // Win32 condition variables were introduced in Windows Vista, so force
    // _WIN32_WINNT and WINVER to 0x0600 to ensure that Vista-specific symbols
    // are declared.
    #if defined(_WIN32_WINNT)
        #undef _WIN32_WINNT
    #endif
    #if defined(WINVER)
        #undef WINVER
    #endif
    #define _WIN32_WINNT 0x0600
    #define WINVER 0x0600

    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif
#endif // MSVCRT_SUPPORT_ENABLE_THREAD_SAFE_STATICS

#if defined(_MSC_VER)
    #pragma warning (push, 0)
#endif
// InitializeCriticalSectionAndSpinCount, DeleteCriticalSection,
// EnterCriticalSection, LeaveCriticalSection, InitializeConditionVariable,
// SleepConditionVariableCS, WakeAllConditionVariable
#include <windows.h>

#include <atomic>
#if defined(_MSC_VER)
    #pragma warning (pop)
#endif

namespace msvcrt_support {

static_assert(sizeof(CRITICAL_SECTION) <= sizeof(mtx_t::opaque), "!");
static_assert(alignof(CRITICAL_SECTION) <= alignof(mtx_t), "!");

using InitializeCriticalSectionAndSpinCount_t = BOOL __stdcall (
    LPCRITICAL_SECTION lpCriticalSection,
    DWORD dwSpinCount);

static std::atomic<InitializeCriticalSectionAndSpinCount_t *> cs_init_sc;

static BOOL __stdcall
stub_InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION cs, DWORD spins) {
    InitializeCriticalSection(cs);
    return TRUE;
}

bool
mtx_init(mtx_t &mtx) {
    auto csinit = cs_init_sc.load(std::memory_order_consume);
    if (csinit == nullptr) {
        auto fp = &stub_InitializeCriticalSectionAndSpinCount;
        auto const hdl = GetModuleHandleW(LR"(system32\kernel32.dll)");
        if (hdl == nullptr) goto next;
        {
            auto const proc = GetProcAddress(hdl, "InitializeCriticalSectionAndSpinCount");
            if (proc == nullptr) goto next;
            fp = reinterpret_cast<InitializeCriticalSectionAndSpinCount_t *>(proc);
            goto next;
        }
        next:
        cs_init_sc.store(fp, std::memory_order_release);
        csinit = fp;
    }
    auto const result = csinit(
        reinterpret_cast<CRITICAL_SECTION *>(mtx.opaque),
        4000);
    return result != 0;
}

void
mtx_uninit(mtx_t &mtx) {
    DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION *>(mtx.opaque));
}

void
mtx_lock(mtx_t &mtx) {
    EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION *>(mtx.opaque));
}

void
mtx_unlock(mtx_t &mtx) {
    LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION *>(mtx.opaque));
}

#if MSVCRT_SUPPORT_ENABLE_THREAD_SAFE_STATICS
static_assert(sizeof(CONDITION_VARIABLE) <= sizeof(condvar_t::opaque), "!");
static_assert(alignof(CONDITION_VARIABLE) <= alignof(condvar_t), "!");

void
condvar_init(condvar_t &cvar) {
    InitializeConditionVariable(
        reinterpret_cast<CONDITION_VARIABLE *>(cvar.opaque));
}

void
condvar_uninit(condvar_t &) {
}

bool
condvar_wait_timeout(condvar_t &cvar, mtx_t &mtx, unsigned __int32 const timeout) {
    auto const result = SleepConditionVariableCS(
        reinterpret_cast<CONDITION_VARIABLE *>(cvar.opaque),
        reinterpret_cast<CRITICAL_SECTION *>(mtx.opaque),
        timeout);
    return result != 0;
}

void
condvar_notify_all(condvar_t &cvar) {
    WakeAllConditionVariable(reinterpret_cast<CONDITION_VARIABLE *>(cvar.opaque));
}
#endif // MSVCRT_SUPPORT_ENABLE_THREAD_SAFE_STATICS

using get_environ_t = errno_t __cdecl (char ***value);

char **internal_environ = nullptr;

static errno_t __cdecl
stub_get_environ(char ***value) {
    *value = internal_environ;
    return 0;
}

std::atomic<get_environ_t *> get_environ_fp;

void
internal_get_environ(char ***value) {
    auto ge = get_environ_fp.load(std::memory_order_consume);
    if (ge == nullptr) {
        {
            auto const msvcrt = GetModuleHandleW(LR"(System32\msvcrt.dll)");
            if (msvcrt == nullptr) return;
            auto const raw = GetProcAddress(msvcrt, "_get_environ");
            if (raw != nullptr) {
                ge = reinterpret_cast<get_environ_t *>(raw);
                goto done;
            }
            auto const rawenv = GetProcAddress(msvcrt, "_environ");
            if (rawenv != nullptr) {
                internal_environ = reinterpret_cast<char **>(rawenv);
                ge = stub_get_environ;
            }
        }
        done:
        get_environ_fp.store(ge, std::memory_order_release);
    }
    ge(value);
}

}

// #Section: Concurrency control stuff used by the VS2017 C++ stdlib headers ---
namespace std {
    struct once_flag { void *_Opaque; };

    using exec_once_func_t = int (__stdcall *) (void *, void *env, void **);

    static auto const Uninitialized = reinterpret_cast<void *>(0x0);
    static auto const Initializing = reinterpret_cast<void *>(0x1);
    static auto const Initialized = reinterpret_cast<void *>(0x2);

    int __cdecl
    _Execute_once(once_flag& _Flag, exec_once_func_t func, void *env) noexcept {
        auto &flag = reinterpret_cast<std::atomic<void *> &>(_Flag._Opaque);
        int count = 0;
        int retval = 0;

        retry:
        auto const val = flag.load(std::memory_order_acquire);
        if (val == Initialized) return retval;
        auto expected = Uninitialized;
        auto const success = flag.compare_exchange_strong(expected, Initializing, std::memory_order_relaxed, std::memory_order_relaxed);
        if (success) {
            retval = func(nullptr, env, nullptr);
            flag.store(Initialized, std::memory_order_release);
        } else {
            // I have no justification for this number.
            if (count < 40) {
                _mm_pause();
            } else {
                Sleep(1);
            }
            ++count;
            goto retry;
        }
        return retval;
    }
}

#include <intrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

// #Section: Floating-point conversion procedures for AMD64 --------------------
//
// SSE2 is guaranteed to be present on AMD64, so it's safe to unconditionally
// use these intrinsics.
#if defined(_M_AMD64)
extern "C" {
    #if !defined(__clang__)
        #pragma function(lrintf)
    #endif
    long __cdecl
    lrintf(float x) {
        auto const xmm = _mm_set_ss(x);
        return _mm_cvt_ss2si(xmm);
    }

    long long __cdecl
    llrintf(float x) {
        auto const xmm = _mm_set_ss(x);
        return _mm_cvt_ss2si(xmm);
    }

    #if !defined(__clang__)
        #pragma function(lrint)
    #endif
    long __cdecl
    lrint(double x) {
        auto const xmm = _mm_set_sd(x);
        return _mm_cvtsd_si32(xmm);
    }

    long long __cdecl
    llrint(double x) {
        auto const xmm = _mm_set_sd(x);
        // also see: _mm_cvttsd_si64x
        return _mm_cvtsd_si64(xmm);
    }
}
#endif

// #Section: Floating-point conversion procedures for i386 ---------------------
//
// x87-compatible FP instructions are guaranteed to be present on Pentiums and
// newer, and I'm not interested in targeting the 386 or 486.
#if defined(_M_IX86)
extern "C" {
    #if !defined(__clang__)
        #pragma function(lrintf)
    #endif
    long __cdecl
    lrintf(float x) {
        long result;
        __asm {
            fld x;
            fistp result;
        };
        return result;
    }

    long long __cdecl
    llrintf(float x) {
        long long result;
        __asm {
            fld x;
            fistp result;
        };
        return result;
    }

    #if !defined(__clang__)
        #pragma function(lrint)
    #endif
    long __cdecl
    lrint(double x) {
        long result;
        __asm {
            fld x;
            fistp result;
        };
        return result;
    }

    long long __cdecl
    llrint(double x) {
        long long result;
        __asm {
            fld x;
            fistp result;
        };
        return result;
    }

    void __declspec(naked)
    _libm_sse2_sqrt_precise() {
        __asm {
            sqrtss xmm0, xmm0
        }
    }
}
#endif

#pragma warning (pop)

#include "msvcrt-support-crt0.implementation.hpp"

#endif // defined(_MSC_VER)
