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
