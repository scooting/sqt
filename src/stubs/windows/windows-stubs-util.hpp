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
#pragma once

#include <QtCore/qglobal.h>

namespace sqt { namespace winapi {
    template <size_t count_, size_t stride_>
    static constexpr size_t Q_ALWAYS_INLINE
    count(char const (&)[count_][stride_]) {
        return count_;
    }

    template <size_t count_, size_t stride_>
    static constexpr size_t Q_ALWAYS_INLINE
    stride(char const (&)[count_][stride_]) {
        return stride_;
    }

    inline static void Q_NEVER_INLINE
    winapi_resolve(
        wchar_t const * const name,
        void ** const out,
        void * const * const stubs,
        char const *symnames,
        size_t const symnames_stride,
        size_t const count)
    {
        auto const mod = LoadLibraryW(name);
        for (size_t i = 0; i < count; ++i) {
            {
                if (mod == nullptr) goto use_stub;
                auto const symname = &symnames[i * symnames_stride];
                auto const fptr = GetProcAddress(mod, symname);
                if (fptr == nullptr) goto use_stub;
                out[i] = (void *) fptr;
            }
            continue;

            use_stub:
            out[i] = stubs[i];
        }
    }
} }

#if defined(__GNUC__)
    #define SQT_UNLIKELY(x) __builtin_expect(!!(x), false)
#else
    #define SQT_UNLIKELY(x) (x)
#endif
