/*
Unless otherwise noted, the code in this file is licensed under the following
license:

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
// This file does not and *must not* include any external header files. The
// MSVC2015 UCRT header files will cause most of the symbols defined in this
// translation unit to be publicly exported, which will cause problems when
// multiple static archives that themselves depend on this static glue library
// are linked together. To avoid this, all necessary symbols are declared and
// defined in this file, and symbols that are too burdensome to redeclare
// manually are isolated in separate translation units. C++: two thumbs down.
#if defined(_MSC_VER)

#include "msvcrt-support-config.hpp"

#include "msvcrt-support-stubs-dirty.hpp"

#pragma warning (disable : 4244) // conversion from 'double' to 'volatile float', possible loss of data
#pragma warning (disable : 4723) // potential divide by 0

#define INT_MIN (-2147483647 - 1)

// #Section: Basic definitions and intrinsics ----------------------------------
//
// These are useful intrinsics that map to widely-supported processor
// instructions (should be `lock xadd`/`lock inc`/`lock dec` on i386 and amd64).
// These are normally declared in intrin.h, but I can't include intrin.h here,
// because that header includes lots of Microsoft CRT headers that would pollute
// this translation unit with declarations I'm deliberately trying to subvert.
extern "C" {
    long
    _InterlockedIncrement(long volatile * _Addend);
    #pragma intrinsic(_InterlockedIncrement)

    long
    _InterlockedDecrement(long volatile * _Addend);
    #pragma intrinsic(_InterlockedDecrement)

    unsigned int
    _mm_getcsr();
    #pragma intrinsic(_mm_getcsr)

    using DWORD = unsigned long;
    using uint64_t = unsigned long long;
    using uint32_t = unsigned int;
    using uint16_t = unsigned short;
    using int32_t = int;
    using u_int32_t = uint32_t;
    using errno_t = int;

    #if !defined(ULLONG_MAX)
        #define ULLONG_MAX 0xffffffffffffffffui64
    #endif

    #if !defined(ERANGE)
        #define ERANGE 34
    #endif

    __declspec(dllimport)
    extern errno_t _errno;

    __declspec(selectany) int _fltused = 0;

    static_assert(sizeof(uint64_t) == 8, "!");
    static_assert(sizeof(uint32_t) == 4, "!");
    static_assert(sizeof(DWORD) == 4, "!");
}


#if defined(_M_AMD64)
    using uintptr_t = uint64_t;
#elif defined(_M_IX86)
    using uintptr_t = uint32_t;
#else
    #error "msvcrt-support is not supported for non-amd64/non-i386."
#endif

#define LOCAL_IMPORT(sym) decltype(sym) * __imp_##sym = sym;

// #Section: UCRT runtime checking ---------------------------------------------
//
// These functions are used by different UCRT headers and are apparently part of
// some sort of parameter validation infrastructure that aborts the program when
// invalid parameters are passed to different UCRT functions. These functions
// are defined here (with non-dllexport visibility) so that programs built
// against the UCRT headers can build without depending on the "real" versions
// of these functions present in the UCRT DLLs.
#if defined(MSVCRT_SUPPORT_ENABLE_CRT_DEBUG)
extern "C" {
    __declspec(noreturn) void
    __fastfail(unsigned int);

    __declspec(noreturn)
    void __cdecl
    _invalid_parameter_noinfo_noreturn() {
        __fastfail(1);
    }

    LOCAL_IMPORT(_invalid_parameter_noinfo_noreturn);

    void __cdecl
    __report_rangecheckfailure() {
        __fastfail(1);
    }

    LOCAL_IMPORT(__report_rangecheckfailure);

    __declspec(noreturn) void __cdecl
    _invalid_parameter(
        wchar_t const * const expression,
        wchar_t const * const function_name,
        wchar_t const * const file_name,
        unsigned int const line_number,
        uintptr_t const reserved)
    {
        (void) expression;
        (void) function_name;
        (void) file_name;
        (void) line_number;
        (void) reserved;
        __debugbreak();
        __fastfail(1);
    }

    __declspec(noreturn) void __cdecl
    _invoke_watson(
        wchar_t const* const expression,
        wchar_t const* const function_name,
        wchar_t const* const file_name,
        unsigned int const line_number,
        uintptr_t const reserved)
    {
        (void) expression;
        (void) function_name;
        (void) file_name;
        (void) line_number;
        (void) reserved;
        __debugbreak();
        __fastfail(1);
    }

    __declspec(noreturn) int __cdecl
   _CrtDbgReport(
        int const report_type,
        const char * const filename,
        int linenumber,
        const char * const module_name,
        const char * const format,
        ...)
    {
        (void) report_type;
        (void) filename;
        (void) linenumber;
        (void) module_name;
        (void) format;
        __debugbreak();
        __fastfail(1);
        __assume(false);
    }
}
#endif // defined(MSVCRT_SUPPORT_ENABLE_CRT_DEBUG)

#if 1
extern "C" {
    __declspec(dllimport)
    extern double __cdecl
    _hypot(double x, double y);

    double __cdecl
    hypot(double x, double y) {
        return _hypot(x, y);
    }
}
#endif

// #Section: Floating point support functions ----------------------------------
//
// The nested braced extern "C"s are here to make it easy to use vi's brace
// matching to find the end of this blob of code. Some code is effectively
// copied and pasted from external sources. External code under a uniform
// license is also grouped together under an extern "C" block.
extern "C" {

extern "C" {
    static short const Denorm = -2;
    static short const Finite = -1;
    static short const Zero = 0;
    static short const Infinite = 1;
    static short const Nan = 2;
    static uint64_t const f64_mantissa = 0x000fffffffffffffull;
    static uint64_t const f64_exponent = 0x7ff0000000000000ull;
    static uint32_t const f32_mantissa = 0x007fffffu;
    static uint32_t const f32_exponent = 0x7f800000u;

    namespace {
        union f64_repr { double f64; uint64_t u64; };
        union f32_repr { float f32; uint32_t u32; };
    }

    #pragma warning (push)
    #pragma warning (disable : 4163) // '...': not available as an intrinsic function

    short __cdecl
    _ldclass(long double);
    #if !defined(__clang__)
        #pragma function(_ldclass)
    #endif

    short __cdecl
    _dclass(double);
    #if !defined(__clang__)
        #pragma function(_dclass)
    #endif

    short __cdecl
    _fdclass(float);
    #if !defined(__clang__)
        #pragma function(_fdclass)
    #endif

    short __cdecl
    _dtest(double *);
    #if !defined(__clang__)
        #pragma function(_dtest)
    #endif

    short __cdecl
    _fdtest(float *);
    #if !defined(__clang__)
        #pragma function(_fdtest)
    #endif

    short __cdecl
    _ldtest(long double *);
    #if !defined(__clang__)
        #pragma function(_ldtest)
    #endif

    #pragma warning (pop)

    short __cdecl
    _ldclass(long double const x) {
        static_assert(sizeof(long double) == sizeof(double), "!");
        return _dclass(x);
    }

    short __cdecl
    _dclass(double const x) {
        f64_repr const repr { x };
        auto const exponent = repr.u64 & f64_exponent;
        auto const mantissa = repr.u64 & f64_mantissa;
        if (exponent == f64_exponent) {
            if (mantissa == 0) return Infinite;
            else return Nan;
        }
        if (exponent == 0) {
            if (mantissa == 0) return Zero;
            else return Denorm;
        }
        return Finite;
    }

    short __cdecl
    _fdclass(float const x) {
        f32_repr const repr { x };
        auto const exponent = repr.u32 & f32_exponent;
        auto const mantissa = repr.u32 & f32_mantissa;
        if (exponent == f32_mantissa) {
            if (mantissa == 0) return Infinite;
            else return Nan;
        }
        if (exponent == 0) {
            if (mantissa == 0) return Zero;
            else return Denorm;
        }
        return Finite;
    }

    short __cdecl
    _ldtest(long double * const x) {
        return _ldclass(*x);
    }

    short __cdecl
    _dtest(double * const x) {
        return _dclass(*x);
    }

    short __cdecl
    _fdtest(float *x) {
        return _fdclass(*x);
    }

    #if defined(_M_AMD64)
    int __cdecl
    __fpe_flt_rounds() {
        // bit 14, bit 13
        // 00 (0x0000) = round to nearest
        // 01 (0x2000) = round negative
        // 10 (0x4000) = round positive
        // 11 (0x6000) = round to zero
        auto const csr = _mm_getcsr();
        auto const roundbits = csr & 0x6000;
        switch (roundbits) {
        case 0x6000: return 0; // FE_TOWARDZERO, towards zero
        case 0x0000: return 1; // FE_TONEAREST, to nearest
        case 0x4000: return 2; // FE_UPWARD, towards positive infinity
        case 0x2000: return 3; // FE_DOWNWARD, towards negative infinity
        }
        __assume(false);
    }
    #elif defined(_M_IX86)
    int __cdecl
    __fpe_flt_rounds() {
        // bit 11, bit 10
        // 00 (0x000) = round to nearest
        // 01 (0x400) = round negative
        // 10 (0x800) = round positive
        // 11 (0xc00) = round to zero
        uint16_t fcw;
        __asm {
            fnstcw fcw;
        }
        auto const roundbits = fcw & 0xc00;
        switch (roundbits) {
        case 0xc00: return 0; // FE_TOWARDZERO, towards zero
        case 0x000: return 1; // FE_TONEAREST, to nearest
        case 0x800: return 2; // FE_UPWARD, towards positive infinity
        case 0x400: return 3; // FE_DOWNWARD, towards negative infinity
        }
        __assume(false);
    }
    #endif

    int __cdecl
    _dsign(double const x) {
        f64_repr const repr { x };
        return static_cast<int>((repr.u64 & 0x8000000000000000ull) >> 48);
    }

    float __cdecl
    copysignf(float const x, float const y) {
        f32_repr const y_repr { y };
        f32_repr x_repr { x };
        x_repr.u32 = (x_repr.u32 & 0x7fffffffu) | (y_repr.u32 & 0x80000000u);
        return x_repr.f32;
    }

    double __cdecl
    copysign(double const x, double const y) {
        f64_repr const y_repr { y };
        f64_repr x_repr { x };
        x_repr.u64 = (x_repr.u64 & 0x7fffffffffffffffllu) | (y_repr.u64 & 0x8000000000000000llu);
        return x_repr.f64;
    }

    static double __forceinline
    asdouble(uint64_t const u64) {
        f64_repr repr;
        repr.u64 = u64;
        return repr.f64;
    }

    static uint64_t __forceinline
    asuint64(double const f64) {
        f64_repr repr;
        repr.f64 = f64;
        return repr.u64;
    }

    static uint64_t infty_repr = 0x7ff0000000000000ull;
    #define INFINITY (asdouble(infty_repr))
}

// The code within this `extern "C"` block is derived from musl libc.
// This code is licensed under the following license:
//
// Copyright (c) 2005-2014 Rich Felker, et al.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
extern "C" {

    #if defined(__GNUC__) || defined(__clang__)
        #define predict_true(x) __builtin_expect(!!(x), 1)
        #define predict_false(x) __builtin_expect(x, 0)
    #else
        #define predict_true(x) (x)
        #define predict_false(x) (x)
    #endif

    #define FLT_EPSILON      1.192092896e-07F   // smallest such that 1.0+FLT_EPSILON != 1.0
    #define DBL_EPSILON 2.2204460492503131e-016 // smallest such that 1.0+DBL_EPSILON != 1.0
    #define WANT_ROUNDING 1

    #define FORCE_EVAL(x) do {                        \
            if (sizeof(x) == sizeof(float)) {         \
                    volatile float __x;               \
                    __x = (x);                        \
            } else if (sizeof(x) == sizeof(double)) { \
                    volatile double __x;              \
                    __x = (x);                        \
            } else {                                  \
                    volatile long double __x;         \
                    __x = (x);                        \
            }                                         \
    } while(0)

    /* Evaluate an expression as the specified type. With standard excess
       precision handling a type cast or assignment is enough (with
       -ffloat-store an assignment is required, in old compilers argument
       passing and return statement may not drop excess precision).  */

    static double __forceinline
    eval_as_double(double x) {
        double y = x;
        return y;
    }

    /* fp_barrier returns its input, but limits code transformations
       as if it had a side-effect (e.g. observable io) and returned
       an arbitrary value.  */

    static double __forceinline
    fp_barrier(double x) {
        volatile double y = x;
        return y;
    }

    /* fp_force_eval ensures that the input value is computed when that's
       otherwise unused.  To prevent the constant folding of the input
       expression, an additional fp_barrier may be needed or a compilation
       mode that does so (e.g. -frounding-math in gcc). Then it can be
       used to evaluate an expression for its fenv side-effects only.   */

    static void __forceinline
    fp_force_eval(double x) {
        volatile double y;
        y = x;
    }

    static const double dbl_toint = 1 / DBL_EPSILON;
    static const double flt_toint = 1 / FLT_EPSILON;

    double __cdecl
    round(double x) {
        union {double f; uint64_t i;} u = {x};
        int e = u.i >> 52 & 0x7ff;
        double y;

        if (e >= 0x3ff+52)
            return x;
        if (u.i >> 63)
            x = -x;
        if (e < 0x3ff-1) {
            /* raise inexact if x!=0 */
            FORCE_EVAL(x + dbl_toint);
            return 0*u.f;
        }
        y = x + dbl_toint - dbl_toint - x;
        if (y > 0.5)
            y = y + x - 1;
        else if (y <= -0.5)
            y = y + x + 1;
        else
            y = y + x;
        if (u.i >> 63)
            y = -y;
        return y;
    }

    long __cdecl
    lround(double x) {
        return static_cast<long>(round(x));
    }

    float __cdecl
    rintf(float x) {
        union {float f; uint32_t i;} u = {x};
        int e = u.i>>23 & 0xff;
        int s = u.i>>31;
        float y;

        if (e >= 0x7f+23)
            return x;
        if (s)
            y = x - flt_toint + flt_toint;
        else
            y = x + flt_toint - flt_toint;
        if (y == 0)
            return s ? -0.0f : 0.0f;
        return y;
    }

    double __cdecl
    rint(double x) {
        union {double f; uint64_t i;} u = {x};
        int e = u.i>>52 & 0x7ff;
        int s = u.i>>63;
        double y;

        if (e >= 0x3ff+52)
            return x;
        if (s)
            y = x - dbl_toint + dbl_toint;
        else
            y = x + dbl_toint - dbl_toint;
        if (y == 0)
            return s ? -0.0 : 0;
        return y;
    }

    /* nearbyint is the same as rint, but it must not raise the inexact exception */

    double __cdecl
    nearbyint(double x) {
    #ifdef FE_INEXACT
        #pragma STDC FENV_ACCESS ON
        int e;

        e = fetestexcept(FE_INEXACT);
    #endif
        x = rint(x);
    #ifdef FE_INEXACT
        if (!e)
            feclearexcept(FE_INEXACT);
    #endif
        return x;
    }

    float __cdecl
    nearbyintf(float x) {
    #ifdef FE_INEXACT
        #pragma STDC FENV_ACCESS ON
        int e;

        e = fetestexcept(FE_INEXACT);
    #endif
        x = rintf(x);
    #ifdef FE_INEXACT
        if (!e)
                feclearexcept(FE_INEXACT);
    #endif
        return x;
    }

    double __cdecl
    trunc(double x) {
        union {double f; uint64_t i;} u = {x};
        int e = (int)(u.i >> 52 & 0x7ff) - 0x3ff + 12;
        uint64_t m;

        if (e >= 52 + 12)
                return x;
        if (e < 12)
                e = 1;
        m = -1ULL >> e;
        if ((u.i & m) == 0)
                return x;
        // FORCE_EVAL(x + 0x1p120f);
        FORCE_EVAL(x + 1.329228e36f);
        u.i &= ~m;
        return u.f;
    }

    float __cdecl
    truncf(float x) {
        union {float f; uint32_t i;} u = {x};
        int e = (int)(u.i >> 23 & 0xff) - 0x7f + 9;
        uint32_t m;

        if (e >= 23 + 9)
                return x;
        if (e < 9)
                e = 1;
        m = -1U >> e;
        if ((u.i & m) == 0)
                return x;
        // FORCE_EVAL(x + 0x1p120f);
        FORCE_EVAL(x + 1.329228e36f);
        u.i &= ~m;
        return u.f;
    }

    // FLT_EVAL_METHOD is 0 for amd64 and i386 MSVC. See musl's alltypes.h.in
    // for more.
    using double_t = double;

    double const f64_max_exp = 8.9884656743115795e+307; // 0x7fe0000000000000, 0x1p1023
    double const f64_min_exp = 2.2250738585072014e-308; // 0x0010000000000000, 0x1p-1022
    double const f64_two53 = 9007199254740992.0; // 0x4340000000000000, 0x1p53

    double __cdecl
    scalbn(double x, int n) {
        union {double f; uint64_t i;} u;
        double_t y = x;

        if (n > 1023) {
                y *= f64_max_exp;
                n -= 1023;
                if (n > 1023) {
                        y *= f64_max_exp;
                        n -= 1023;
                        if (n > 1023)
                                n = 1023;
                }
        } else if (n < -1022) {
                /* make sure final n < -53 to avoid double
                   rounding in the subnormal range */
                y *= f64_min_exp * f64_two53;
                n += 1022 - 53;
                if (n < -1022) {
                        y *= f64_min_exp * f64_two53;
                        n += 1022 - 53;
                        if (n < -1022)
                                n = -1022;
                }
        }
        u.i = (uint64_t)(0x3ff+n)<<52;
        x = y * u.f;
        return x;
    }
}

// The code within this `extern "C"` block is derived from musl's copy of
// FreeBSD's log2. The code is licensed under the following license:
//
// ====================================================
// Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
//
// Developed at SunSoft, a Sun Microsystems, Inc. business.
// Permission to use, copy, modify, and distribute this
// software is freely granted, provided that this notice
// is preserved.
// ====================================================
extern "C" {
    static const double
    two54 = 1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */ // 0x1p54
    ivln2hi = 1.44269504072144627571e+00, /* 0x3ff71547, 0x65200000 */
    ivln2lo = 1.67517131648865118353e-10, /* 0x3de705fc, 0x2eefa200 */
    Lg1 = 6.666666666666735130e-01,  /* 3FE55555 55555593 */
    Lg2 = 3.999999999940941908e-01,  /* 3FD99999 9997FA04 */
    Lg3 = 2.857142874366239149e-01,  /* 3FD24924 94229359 */
    Lg4 = 2.222219843214978396e-01,  /* 3FCC71C5 1D8E78AF */
    Lg5 = 1.818357216161805012e-01,  /* 3FC74664 96CB03DE */
    Lg6 = 1.531383769920937332e-01,  /* 3FC39A09 D078C69F */
    Lg7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */

    double __cdecl
    log2(double x);
    #if !defined(__clang__)
        #pragma function(log2)
    #endif

    double __cdecl
    log2(double x) {
            union {double f; uint64_t i;} u = {x};
            double hfsq,f,s,z,R,w,t1,t2,y,hi,lo,val_hi,val_lo;
            uint32_t hx;
            int k;

            hx = u.i>>32;
            k = 0;
            if (hx < 0x00100000 || hx>>31) {
                    if (u.i<<1 == 0)
                            return -1/(x*x);  /* log(+-0)=-inf */
                    if (hx>>31)
                            return (x-x)/0.0; /* log(-#) = NaN */
                    /* subnormal number, scale x up */
                    k -= 54;
                    x *= two54;
                    u.f = x;
                    hx = u.i>>32;
            } else if (hx >= 0x7ff00000) {
                    return x;
            } else if (hx == 0x3ff00000 && u.i<<32 == 0)
                    return 0;

            /* reduce x into [sqrt(2)/2, sqrt(2)] */
            hx += 0x3ff00000 - 0x3fe6a09e;
            k += (int)(hx>>20) - 0x3ff;
            hx = (hx&0x000fffff) + 0x3fe6a09e;
            u.i = (uint64_t)hx<<32 | (u.i&0xffffffff);
            x = u.f;

            f = x - 1.0;
            hfsq = 0.5*f*f;
            s = f/(2.0+f);
            z = s*s;
            w = z*z;
            t1 = w*(Lg2+w*(Lg4+w*Lg6));
            t2 = z*(Lg1+w*(Lg3+w*(Lg5+w*Lg7)));
            R = t2 + t1;

            /*
             * f-hfsq must (for args near 1) be evaluated in extra precision
             * to avoid a large cancellation when x is near sqrt(2) or 1/sqrt(2).
             * This is fairly efficient since f-hfsq only depends on f, so can
             * be evaluated in parallel with R.  Not combining hfsq with R also
             * keeps R small (though not as small as a true `lo' term would be),
             * so that extra precision is not needed for terms involving R.
             *
             * Compiler bugs involving extra precision used to break Dekker's
             * theorem for spitting f-hfsq as hi+lo, unless double_t was used
             * or the multi-precision calculations were avoided when double_t
             * has extra precision.  These problems are now automatically
             * avoided as a side effect of the optimization of combining the
             * Dekker splitting step with the clear-low-bits step.
             *
             * y must (for args near sqrt(2) and 1/sqrt(2)) be added in extra
             * precision to avoid a very large cancellation when x is very near
             * these values.  Unlike the above cancellations, this problem is
             * specific to base 2.  It is strange that adding +-1 is so much
             * harder than adding +-ln2 or +-log10_2.
             *
             * This uses Dekker's theorem to normalize y+val_hi, so the
             * compiler bugs are back in some configurations, sigh.  And I
             * don't want to used double_t to avoid them, since that gives a
             * pessimization and the support for avoiding the pessimization
             * is not yet available.
             *
             * The multi-precision calculations for the multiplications are
             * routine.
             */

            /* hi+lo = f - hfsq + s*(hfsq+R) ~ log(1+f) */
            hi = f - hfsq;
            u.f = hi;
            u.i &= (uint64_t)-1<<32;
            hi = u.f;
            lo = f - hi - hfsq + s*(hfsq+R);

            val_hi = hi*ivln2hi;
            val_lo = (lo+hi)*ivln2lo + lo*ivln2hi;

            /* spadd(val_hi, val_lo, y), except for not using double_t: */
            y = k;
            w = y + val_hi;
            val_lo += (y - w) + val_hi;
            val_hi = w;

            return val_lo + val_hi;
    }
}

// exp2 is derived from musl's exp2.c.
// This code is licensed under the following license:
//
// Copyright (c) 1999-2018, Arm Limited.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
extern "C" {
    static double __declspec(noinline)
    __math_xflow(uint32_t sign, double y) {
        return eval_as_double(fp_barrier(sign ? -y : y) * y);
    }

    static double
    __math_uflow(uint32_t sign) {
        // return __math_xflow(sign, 0x1p-767);
        return __math_xflow(sign, 1.2882297539194267e-231); // 0x1000000000000000, 0x1p-767
    }

    static double
    __math_oflow(uint32_t sign) {
        // return __math_xflow(sign, 0x1p769);
        return __math_xflow(sign, 3.1050361846014179e+231); // 0x7000000000000000, 0x1p769
    }

    #define EXP_TABLE_BITS 7
    #define EXP_POLY_ORDER 5
    #define EXP_USE_TOINT_NARROW 0
    #define EXP2_POLY_ORDER 5
    struct exp_data {
        double invln2N;
        double shift;
        double negln2hiN;
        double negln2loN;
        double poly[4]; /* Last four coefficients.  */
        double exp2_shift;
        double exp2_poly[EXP2_POLY_ORDER];
        uint64_t tab[2*(1 << EXP_TABLE_BITS)];
    };

    #define N (1 << EXP_TABLE_BITS)

    exp_data const __exp_data = {
        // N/ln2
        // .invln2N = 0x1.71547652b82fep0 * N,
        1.4426950408889634,

        // -ln2/N
        //.negln2hiN = -0x1.62e42fefa0000p-8,
        //.negln2loN = -0x1.cf79abc9e3b3ap-47,
        -0.0054152123481117087,
        -1.2864023111638346e-14,

        // Used for rounding when !TOINT_INTRINSICS
        #if EXP_USE_TOINT_NARROW
        .shift = 0x1800000000.8p0,
        #else
        // .shift = 0x1.8p52,
        6755399441055744.0,
        #endif

        // exp polynomial coefficients.
        /* .poly = */ {
        // abs error: 1.555*2^-66
        // ulp error: 0.509 (0.511 without fma)
        // if |x| < ln2/256+eps
        // abs error if |x| < ln2/256+0x1p-15: 1.09*2^-65
        // abs error if |x| < ln2/128: 1.7145*2^-56
        // 0x1.ffffffffffdbdp-2,
        // 0x1.555555555543cp-3,
        // 0x1.55555cf172b91p-5,
        // 0x1.1111167a4d017p-7,

        0.49999999999996786,
        0.16666666666665886,
        0.041666680841067401,
        0.008333335853059549,
        },

        // .exp2_shift = 0x1.8p52 / N,
        6755399441055744.0 / N,

        // exp2 polynomial coefficients.
        /* .exp2_poly = */ {
        // abs error: 1.2195*2^-65
        // ulp error: 0.507 (0.511 without fma)
        // if |x| < 1/256
        // abs error if |x| < 1/128: 1.9941*2^-56
        // 0x1.62e42fefa39efp-1,
        // 0x1.ebfbdff82c424p-3,
        // 0x1.c6b08d70cf4b5p-5,
        // 0x1.3b2abd24650ccp-7,
        // 0x1.5d7e09b4e3a84p-10,

        0.69314718055994529,
        0.24022650695909065,
        0.0555041086686087,
        0.0096181319757210545,
        0.0013332074570119598,
        },
        // 2^(k/N) ~= H[k]*(1 + T[k]) for int k in [0,N)
        // tab[2*k] = asuint64(T[k])
        // tab[2*k+1] = asuint64(H[k]) - (k << 52)/N
        /* .tab = */ {
        0x0, 0x3ff0000000000000,
        0x3c9b3b4f1a88bf6e, 0x3feff63da9fb3335,
        0xbc7160139cd8dc5d, 0x3fefec9a3e778061,
        0xbc905e7a108766d1, 0x3fefe315e86e7f85,
        0x3c8cd2523567f613, 0x3fefd9b0d3158574,
        0xbc8bce8023f98efa, 0x3fefd06b29ddf6de,
        0x3c60f74e61e6c861, 0x3fefc74518759bc8,
        0x3c90a3e45b33d399, 0x3fefbe3ecac6f383,
        0x3c979aa65d837b6d, 0x3fefb5586cf9890f,
        0x3c8eb51a92fdeffc, 0x3fefac922b7247f7,
        0x3c3ebe3d702f9cd1, 0x3fefa3ec32d3d1a2,
        0xbc6a033489906e0b, 0x3fef9b66affed31b,
        0xbc9556522a2fbd0e, 0x3fef9301d0125b51,
        0xbc5080ef8c4eea55, 0x3fef8abdc06c31cc,
        0xbc91c923b9d5f416, 0x3fef829aaea92de0,
        0x3c80d3e3e95c55af, 0x3fef7a98c8a58e51,
        0xbc801b15eaa59348, 0x3fef72b83c7d517b,
        0xbc8f1ff055de323d, 0x3fef6af9388c8dea,
        0x3c8b898c3f1353bf, 0x3fef635beb6fcb75,
        0xbc96d99c7611eb26, 0x3fef5be084045cd4,
        0x3c9aecf73e3a2f60, 0x3fef54873168b9aa,
        0xbc8fe782cb86389d, 0x3fef4d5022fcd91d,
        0x3c8a6f4144a6c38d, 0x3fef463b88628cd6,
        0x3c807a05b0e4047d, 0x3fef3f49917ddc96,
        0x3c968efde3a8a894, 0x3fef387a6e756238,
        0x3c875e18f274487d, 0x3fef31ce4fb2a63f,
        0x3c80472b981fe7f2, 0x3fef2b4565e27cdd,
        0xbc96b87b3f71085e, 0x3fef24dfe1f56381,
        0x3c82f7e16d09ab31, 0x3fef1e9df51fdee1,
        0xbc3d219b1a6fbffa, 0x3fef187fd0dad990,
        0x3c8b3782720c0ab4, 0x3fef1285a6e4030b,
        0x3c6e149289cecb8f, 0x3fef0cafa93e2f56,
        0x3c834d754db0abb6, 0x3fef06fe0a31b715,
        0x3c864201e2ac744c, 0x3fef0170fc4cd831,
        0x3c8fdd395dd3f84a, 0x3feefc08b26416ff,
        0xbc86a3803b8e5b04, 0x3feef6c55f929ff1,
        0xbc924aedcc4b5068, 0x3feef1a7373aa9cb,
        0xbc9907f81b512d8e, 0x3feeecae6d05d866,
        0xbc71d1e83e9436d2, 0x3feee7db34e59ff7,
        0xbc991919b3ce1b15, 0x3feee32dc313a8e5,
        0x3c859f48a72a4c6d, 0x3feedea64c123422,
        0xbc9312607a28698a, 0x3feeda4504ac801c,
        0xbc58a78f4817895b, 0x3feed60a21f72e2a,
        0xbc7c2c9b67499a1b, 0x3feed1f5d950a897,
        0x3c4363ed60c2ac11, 0x3feece086061892d,
        0x3c9666093b0664ef, 0x3feeca41ed1d0057,
        0x3c6ecce1daa10379, 0x3feec6a2b5c13cd0,
        0x3c93ff8e3f0f1230, 0x3feec32af0d7d3de,
        0x3c7690cebb7aafb0, 0x3feebfdad5362a27,
        0x3c931dbdeb54e077, 0x3feebcb299fddd0d,
        0xbc8f94340071a38e, 0x3feeb9b2769d2ca7,
        0xbc87deccdc93a349, 0x3feeb6daa2cf6642,
        0xbc78dec6bd0f385f, 0x3feeb42b569d4f82,
        0xbc861246ec7b5cf6, 0x3feeb1a4ca5d920f,
        0x3c93350518fdd78e, 0x3feeaf4736b527da,
        0x3c7b98b72f8a9b05, 0x3feead12d497c7fd,
        0x3c9063e1e21c5409, 0x3feeab07dd485429,
        0x3c34c7855019c6ea, 0x3feea9268a5946b7,
        0x3c9432e62b64c035, 0x3feea76f15ad2148,
        0xbc8ce44a6199769f, 0x3feea5e1b976dc09,
        0xbc8c33c53bef4da8, 0x3feea47eb03a5585,
        0xbc845378892be9ae, 0x3feea34634ccc320,
        0xbc93cedd78565858, 0x3feea23882552225,
        0x3c5710aa807e1964, 0x3feea155d44ca973,
        0xbc93b3efbf5e2228, 0x3feea09e667f3bcd,
        0xbc6a12ad8734b982, 0x3feea012750bdabf,
        0xbc6367efb86da9ee, 0x3fee9fb23c651a2f,
        0xbc80dc3d54e08851, 0x3fee9f7df9519484,
        0xbc781f647e5a3ecf, 0x3fee9f75e8ec5f74,
        0xbc86ee4ac08b7db0, 0x3fee9f9a48a58174,
        0xbc8619321e55e68a, 0x3fee9feb564267c9,
        0x3c909ccb5e09d4d3, 0x3feea0694fde5d3f,
        0xbc7b32dcb94da51d, 0x3feea11473eb0187,
        0x3c94ecfd5467c06b, 0x3feea1ed0130c132,
        0x3c65ebe1abd66c55, 0x3feea2f336cf4e62,
        0xbc88a1c52fb3cf42, 0x3feea427543e1a12,
        0xbc9369b6f13b3734, 0x3feea589994cce13,
        0xbc805e843a19ff1e, 0x3feea71a4623c7ad,
        0xbc94d450d872576e, 0x3feea8d99b4492ed,
        0x3c90ad675b0e8a00, 0x3feeaac7d98a6699,
        0x3c8db72fc1f0eab4, 0x3feeace5422aa0db,
        0xbc65b6609cc5e7ff, 0x3feeaf3216b5448c,
        0x3c7bf68359f35f44, 0x3feeb1ae99157736,
        0xbc93091fa71e3d83, 0x3feeb45b0b91ffc6,
        0xbc5da9b88b6c1e29, 0x3feeb737b0cdc5e5,
        0xbc6c23f97c90b959, 0x3feeba44cbc8520f,
        0xbc92434322f4f9aa, 0x3feebd829fde4e50,
        0xbc85ca6cd7668e4b, 0x3feec0f170ca07ba,
        0x3c71affc2b91ce27, 0x3feec49182a3f090,
        0x3c6dd235e10a73bb, 0x3feec86319e32323,
        0xbc87c50422622263, 0x3feecc667b5de565,
        0x3c8b1c86e3e231d5, 0x3feed09bec4a2d33,
        0xbc91bbd1d3bcbb15, 0x3feed503b23e255d,
        0x3c90cc319cee31d2, 0x3feed99e1330b358,
        0x3c8469846e735ab3, 0x3feede6b5579fdbf,
        0xbc82dfcd978e9db4, 0x3feee36bbfd3f37a,
        0x3c8c1a7792cb3387, 0x3feee89f995ad3ad,
        0xbc907b8f4ad1d9fa, 0x3feeee07298db666,
        0xbc55c3d956dcaeba, 0x3feef3a2b84f15fb,
        0xbc90a40e3da6f640, 0x3feef9728de5593a,
        0xbc68d6f438ad9334, 0x3feeff76f2fb5e47,
        0xbc91eee26b588a35, 0x3fef05b030a1064a,
        0x3c74ffd70a5fddcd, 0x3fef0c1e904bc1d2,
        0xbc91bdfbfa9298ac, 0x3fef12c25bd71e09,
        0x3c736eae30af0cb3, 0x3fef199bdd85529c,
        0x3c8ee3325c9ffd94, 0x3fef20ab5fffd07a,
        0x3c84e08fd10959ac, 0x3fef27f12e57d14b,
        0x3c63cdaf384e1a67, 0x3fef2f6d9406e7b5,
        0x3c676b2c6c921968, 0x3fef3720dcef9069,
        0xbc808a1883ccb5d2, 0x3fef3f0b555dc3fa,
        0xbc8fad5d3ffffa6f, 0x3fef472d4a07897c,
        0xbc900dae3875a949, 0x3fef4f87080d89f2,
        0x3c74a385a63d07a7, 0x3fef5818dcfba487,
        0xbc82919e2040220f, 0x3fef60e316c98398,
        0x3c8e5a50d5c192ac, 0x3fef69e603db3285,
        0x3c843a59ac016b4b, 0x3fef7321f301b460,
        0xbc82d52107b43e1f, 0x3fef7c97337b9b5f,
        0xbc892ab93b470dc9, 0x3fef864614f5a129,
        0x3c74b604603a88d3, 0x3fef902ee78b3ff6,
        0x3c83c5ec519d7271, 0x3fef9a51fbc74c83,
        0xbc8ff7128fd391f0, 0x3fefa4afa2a490da,
        0xbc8dae98e223747d, 0x3fefaf482d8e67f1,
        0x3c8ec3bc41aa2008, 0x3fefba1bee615a27,
        0x3c842b94c3a9eb32, 0x3fefc52b376bba97,
        0x3c8a64a931d185ee, 0x3fefd0765b6e4540,
        0xbc8e37bae43be3ed, 0x3fefdbfdad9cbe14,
        0x3c77893b4d91cd9d, 0x3fefe7c1819e90d8,
        0x3c5305c14160cc89, 0x3feff3c22b8f71f1,
        },
    };

    #define Shift __exp_data.exp2_shift
    #define T __exp_data.tab
    #define C1 __exp_data.exp2_poly[0]
    #define C2 __exp_data.exp2_poly[1]
    #define C3 __exp_data.exp2_poly[2]
    #define C4 __exp_data.exp2_poly[3]
    #define C5 __exp_data.exp2_poly[4]

    /* Handle cases that may overflow or underflow when computing the result that
       is scale*(1+TMP) without intermediate rounding.  The bit representation of
       scale is in SBITS, however it has a computed exponent that may have
       overflown into the sign bit so that needs to be adjusted before using it as
       a double.  (int32_t)KI is the k used in the argument reduction and exponent
       adjustment of scale, positive k here means the result may overflow and
       negative k means the result may underflow.  */
    static inline double
    specialcase(double_t tmp, uint64_t sbits, uint64_t ki) {
        double_t scale, y;

        if ((ki & 0x80000000) == 0) {
            /* k > 0, the exponent of scale might have overflowed by 1.  */
            sbits -= 1ull << 52;
            scale = asdouble(sbits);
            y = 2 * (scale + scale * tmp);
            return eval_as_double(y);
        }
        /* k < 0, need special care in the subnormal range.  */
        sbits += 1022ull << 52;
        scale = asdouble(sbits);
        y = scale + scale * tmp;
        if (y < 1.0) {
            /* Round y to the right precision before scaling it into the subnormal
               range to avoid double rounding that can cause 0.5+E/2 ulp error where
               E is the worst-case ulp error outside the subnormal range.  So this
               is only useful if the goal is better than 1 ulp worst-case error.  */
            double_t hi, lo;
            lo = scale - y + scale * tmp;
            hi = 1.0 + y;
            lo = 1.0 - hi + y + lo;
            y = eval_as_double(hi + lo) - 1.0;
            /* Avoid -0.0 with downward rounding.  */
            if (WANT_ROUNDING && y == 0.0)
                y = 0.0;
            /* The underflow exception needs to be signaled explicitly.  */
            fp_force_eval(fp_barrier(f64_min_exp) * f64_min_exp);
        }
        y = f64_min_exp * y;
        return eval_as_double(y);
    }

    /* Top 12 bits of a double (sign and exponent bits).  */
    static inline uint32_t
    top12(double x) {
        return asuint64(x) >> 52;
    }

    double const f64_two_neg54 = 5.5511151231257827e-17; // 0x3c90000000000000, 0x1p-54

    double
    exp2(double x) {
        uint32_t abstop;
        uint64_t ki, idx, top, sbits;
        double_t kd, r, r2, scale, tail, tmp;

        abstop = top12(x) & 0x7ff;
        if (predict_false(abstop - top12(f64_two_neg54) >= top12(512.0) - top12(f64_two_neg54))) {
            if (abstop - top12(f64_two_neg54) >= 0x80000000)
                /* Avoid spurious underflow for tiny x.  */
                /* Note: 0 is common input.  */
                return WANT_ROUNDING ? 1.0 + x : 1.0;
            if (abstop >= top12(1024.0)) {
                if (asuint64(x) == asuint64(-INFINITY))
                    return 0.0;
                if (abstop >= top12(INFINITY))
                    return 1.0 + x;
                if (!(asuint64(x) >> 63))
                    return __math_oflow(0);
                else if (asuint64(x) >= asuint64(-1075.0))
                    return __math_uflow(0);
            }
            if (2 * asuint64(x) > 2 * asuint64(928.0))
                /* Large x is special cased below.  */
                abstop = 0;
        }

        /* exp2(x) = 2^(k/N) * 2^r, with 2^r in [2^(-1/2N),2^(1/2N)].  */
        /* x = k/N + r, with int k and r in [-1/2N, 1/2N].  */
        kd = eval_as_double(x + Shift);
        ki = asuint64(kd); /* k.  */
        kd -= Shift; /* k/N for int k.  */
        r = x - kd;
        /* 2^(k/N) ~= scale * (1 + tail).  */
        idx = 2 * (ki % N);
        top = ki << (52 - EXP_TABLE_BITS);
        tail = asdouble(T[idx]);
        /* This is only a valid scale when -1023*N < k < 1024*N.  */
        sbits = T[idx + 1] + top;
        /* exp2(x) = 2^(k/N) * 2^r ~= scale + scale * (tail + 2^r - 1).  */
        /* Evaluation is optimized assuming superscalar pipelined execution.  */
        r2 = r * r;
        /* Without fma the worst case error is 0.5/N ulp larger.  */
        /* Worst case error is less than 0.5+0.86/N+(abs poly error * 2^53) ulp.  */
        tmp = tail + r * C1 + r2 * (C2 + r * C3) + r2 * r2 * (C4 + r * C5);
        if (predict_false(abstop == 0))
            return specialcase(tmp, sbits, ki);
        scale = asdouble(sbits);
        /* Note: tmp == 0 or |tmp| > 2^-65 and scale > 2^-928, so there
           is no spurious underflow here even without fma.  */
        return eval_as_double(scale + scale * tmp);
    }
}


// This implementation of cbrt is licensed under the following license:
//
// Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
//
// Developed at SunPro, a Sun Microsystems, Inc. business.
// Permission to use, copy, modify, and distribute this
// software is freely granted, provided that this notice
// is preserved.
extern "C" {
    static const uint32_t
    B1 = 715094163, /* B1 = (1023-1023/3-0.03306235651)*2**20 */
    B2 = 696219795; /* B2 = (1023-1023/3-54/3-0.03306235651)*2**20 */

    /* |1/cbrt(x) - p(x)| < 2**-23.5 (~[-7.93e-8, 7.929e-8]). */
    static const double
    P0 =  1.87595182427177009643,  /* 0x3ffe03e6, 0x0f61e692 */
    P1 = -1.88497979543377169875,  /* 0xbffe28e0, 0x92f02420 */
    P2 =  1.621429720105354466140, /* 0x3ff9f160, 0x4a49d6c2 */
    P3 = -0.758397934778766047437, /* 0xbfe844cb, 0xbee751d9 */
    P4 =  0.145996192886612446982; /* 0x3fc2b000, 0xd4e4edd7 */

    double __cdecl
    cbrt(double x) {
        union {double f; uint64_t i;} u = {x};
        double_t r,s,t,w;
        uint32_t hx = u.i>>32 & 0x7fffffff;

        if (hx >= 0x7ff00000)  /* cbrt(NaN,INF) is itself */
            return x+x;

        /*
         * Rough cbrt to 5 bits:
         *    cbrt(2**e*(1+m) ~= 2**(e/3)*(1+(e%3+m)/3)
         * where e is integral and >= 0, m is real and in [0, 1), and "/" and
         * "%" are integer division and modulus with rounding towards minus
         * infinity.  The RHS is always >= the LHS and has a maximum relative
         * error of about 1 in 16.  Adding a bias of -0.03306235651 to the
         * (e%3+m)/3 term reduces the error to about 1 in 32. With the IEEE
         * floating point representation, for finite positive normal values,
         * ordinary integer divison of the value in bits magically gives
         * almost exactly the RHS of the above provided we first subtract the
         * exponent bias (1023 for doubles) and later add it back.  We do the
         * subtraction virtually to keep e >= 0 so that ordinary integer
         * division rounds towards minus infinity; this is also efficient.
         */
        if (hx < 0x00100000) { /* zero or subnormal? */
            u.f = x*two54;
            hx = u.i>>32 & 0x7fffffff;
            if (hx == 0)
                return x;  /* cbrt(0) is itself */
            hx = hx/3 + B2;
        } else
                hx = hx/3 + B1;
        u.i &= 1ULL<<63;
        u.i |= (uint64_t)hx << 32;
        t = u.f;

        /*
         * New cbrt to 23 bits:
         *    cbrt(x) = t*cbrt(x/t**3) ~= t*P(t**3/x)
         * where P(r) is a polynomial of degree 4 that approximates 1/cbrt(r)
         * to within 2**-23.5 when |r - 1| < 1/10.  The rough approximation
         * has produced t such than |t/cbrt(x) - 1| ~< 1/32, and cubing this
         * gives us bounds for r = t**3/x.
         *
         * Try to optimize for parallel evaluation as in __tanf.c.
         */
        r = (t*t)*(t/x);
        t = t*((P0+r*(P1+r*P2))+((r*r)*r)*(P3+r*P4));

        /*
         * Round t away from zero to 23 bits (sloppily except for ensuring that
         * the result is larger in magnitude than cbrt(x) but not much more than
         * 2 23-bit ulps larger).  With rounding towards zero, the error bound
         * would be ~5/6 instead of ~4/6.  With a maximum error of 2 23-bit ulps
         * in the rounded t, the infinite-precision error in the Newton
         * approximation barely affects third digit in the final error
         * 0.667; the error in the rounded t can be up to about 3 23-bit ulps
         * before the final error is larger than 0.667 ulps.
         */
        u.f = t;
        u.i = (u.i + 0x80000000) & 0xffffffffc0000000ULL;
        t = u.f;

        /* one step Newton iteration to 53 bits with error < 0.667 ulps */
        s = t*t;         /* t*t is exact */
        r = x/s;         /* error <= 0.5 ulps; |r| < |t| */
        w = t+t;         /* t+t is exact */
        r = (r-t)/(w+r); /* r-t is exact; w+r ~= 3*t */
        t = t+t*r;       /* error <= 0.5 + 0.5/3 + epsilon */
        return t;
    }
}
} // floating point support


// #Section: The usual zero-terminated string garbage --------------------------
//
// This is only here because Qt depends on it.
#if 1
extern "C" {
    __declspec(dllimport)
    extern char * __cdecl
    _strdup(char const * zstr);

    char * __cdecl
    strdup(char const * zstr) {
        return _strdup(zstr);
    }

    static bool
    msvcrt_support_isspace(char const ch) {
        return ch == ' ' || ch == '\t';
    }

    static bool
    msvcrt_support_isdigit(char const ch) {
        return ch >= '0' && ch <= '9';
    }

    static bool
    msvcrt_support_isalpha(char const ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
    }

    static bool
    msvcrt_support_isupper(char const ch) {
        return ch >= 'A' && ch <= 'Z';
    }

    // The code within this `extern "C"` block is derived from revision
    // 872b698bd4a1bfc0bf008c09228e6fd238809c75 of the FreeBSD project's
    // crypto/openssh/openbsd-compat/strtoull.c. This code is licensed under the
    // following license:
    //
    // Copyright (c) 1992 The Regents of the University of California.
    // All rights reserved.
    //
    // Redistribution and use in source and binary forms, with or without
    // modification, are permitted provided that the following conditions
    // are met:
    // 1. Redistributions of source code must retain the above copyright
    //    notice, this list of conditions and the following disclaimer.
    // 2. Redistributions in binary form must reproduce the above copyright
    //    notice, this list of conditions and the following disclaimer in the
    //    documentation and/or other materials provided with the distribution.
    // 3. Neither the name of the University nor the names of its contributors
    //    may be used to endorse or promote products derived from this software
    //    without specific prior written permission.
    //
    // THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
    // ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    // IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    // ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
    // FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    // DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    // OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    // HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    // LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    // OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    // SUCH DAMAGE.
    extern "C" {
        unsigned long long __cdecl
        strtoull(char const * nptr, char ** endptr, int base) {
            const char *s;
            unsigned long long acc, cutoff;
            int c;
            int neg, any, cutlim;

            /*
             * See strtoq for comments as to the logic used.
             */
            s = nptr;
            do {
                    c = (unsigned char) *s++;
            } while (msvcrt_support_isspace(c));
            if (c == '-') {
                    neg = 1;
                    c = *s++;
            } else {
                    neg = 0;
                    if (c == '+')
                            c = *s++;
            }
            if ((base == 0 || base == 16) &&
                c == '0' && (*s == 'x' || *s == 'X')) {
                    c = s[1];
                    s += 2;
                    base = 16;
            }
            if (base == 0)
                    base = c == '0' ? 8 : 10;

            cutoff = ULLONG_MAX / (unsigned long long)base;
            cutlim = ULLONG_MAX % (unsigned long long)base;
            for (acc = 0, any = 0;; c = (unsigned char) *s++) {
                    if (msvcrt_support_isdigit(c))
                            c -= '0';
                    else if (msvcrt_support_isalpha(c))
                            c -= msvcrt_support_isupper(c) ? 'A' - 10 : 'a' - 10;
                    else
                            break;
                    if (c >= base)
                            break;
                    if (any < 0)
                            continue;
                    if (acc > cutoff || (acc == cutoff && c > cutlim)) {
                            any = -1;
                            acc = ULLONG_MAX;
                            _errno = ERANGE;
                    } else {
                            any = 1;
                            acc *= (unsigned long long)base;
                            acc += c;
                    }
            }
            if (neg && any > 0)
                    acc = -acc;
            if (endptr != 0)
                    *endptr = (char *) (any ? s - 1 : nptr);
            return acc;
        }
    }
}
#endif

// #Section: Environment variable support --------------------------------------
#if 1 && (defined(_M_AMD64) || 0)
extern "C" {
    __declspec(dllimport)
    extern errno_t __cdecl
    _get_environ(char*** Value);

    static union {
        char **internal_environ;
        char ** volatile internal_environ_;
    };


    char *** __cdecl
    __p__environ() {
        // TODO: This seems pretty fishy: while the environment variable pointer
        // shouldn't change over the course of execution in a single program, it
        // is still possible for this to produce a data race when multiple
        // threads use this function at the same time.
        char **ret;
        #if 0
        _get_environ(&ret);
        #else
        msvcrt_support::internal_get_environ(&ret);
        #endif
        internal_environ_ = ret;
        return &internal_environ;
    }

    __declspec(dllimport)
    extern int __argc;

    __declspec(dllimport)
    extern char **__argv;

    int * __cdecl
    __p___argc() {
        return &__argc;
    }

    char *** __cdecl
    __p___argv() {
        return &__argv;
    }
}
#endif

// #Section: "C" stdio support -------------------------------------------------
#if defined(MSVCRT_SUPPORT_ENABLE_C_STDIO)
extern "C" {
    #include <vadefs.h> // va_list, __crt_va_start, __crt_va_arg, __crt_va_end

    struct _iobuf {
        char *_ptr;
        int _cnt;
        char *_base;
        int _flag;
        int _file;
        int _charbuf;
        int _bufsiz;
        char *_tmpfname;
    };

    using FILE = _iobuf;

    #define va_start __crt_va_start
    #define va_arg   __crt_va_arg
    #define va_end   __crt_va_end

    struct __crt_locale_pointers;

    using _locale_t = __crt_locale_pointers *;

    #if defined(_M_IX86)
    __declspec(dllimport)
    extern FILE * _iob;

    FILE * __cdecl
    __iob_func() {
        return _iob;
    }
    #else
    __declspec(dllimport)
    extern FILE * __cdecl
    __iob_func();
    #endif

    FILE * __cdecl
    __acrt_iob_func(unsigned ix) {
        auto const iob = __iob_func();
        auto const ret = iob + ix;
        return ret;
    }

    __declspec(dllimport)
    extern long __cdecl
    ftell(FILE *stream);

    __int64 __cdecl
    _ftelli64(FILE *stream) {
        // TODO: Figure out if it's possible to implement _ftelli64 correctly
        // using msvcrt.dll. The definition of `_iobuf` suggests that it might
        // not be possible in a straightforward way.
        return ftell(stream);
    }

    extern int __cdecl
    vfprintf(FILE *, char const *, va_list);

    extern int __cdecl
    _vsnwprintf_l(
        wchar_t * const buf,
        size_t const count,
        wchar_t const * const fmt,
        _locale_t const locale,
        va_list args);


    extern int __cdecl
    _vsnprintf_s_l(
        char * const buf,
        size_t const count,
        size_t const max_count,
        char const * const fmt,
        _locale_t const locale,
        va_list args);

    extern int __cdecl
    _vsnprintf_l(
        char* const buf,
        size_t const count,
        char const * const fmt,
        _locale_t const locale,
        va_list args);

    #if !defined(_TRUNCATE)
        #define _TRUNCATE ((size_t) -1)
    #endif

    #if 0 // Not available in NT 5.2's msvcrt.dll.
    extern int __cdecl
    _vsnprintf_s(
        char * const buf,
        size_t const count,
        size_t const max_count,
        char const * const fmt,
        va_list args);
    #endif

    extern int __cdecl
    _vsnprintf(
        char * const buf,
        size_t const count,
        char const * const fmt,
        va_list args);

    int __cdecl
    vsnprintf(
        char * const buf,
        size_t const count,
        char const * const fmt,
        va_list args)
    {
        return _vsnprintf(buf, count, fmt, args);
    }

    int __cdecl
    vsnprintf_s(
        char * const buf,
        size_t const count,
        size_t const max_count,
        char const * const fmt,
        va_list args)
    {
        size_t _count = count == _TRUNCATE ? max_count : count;
        _count = _count > max_count ? max_count : _count;
        return _vsnprintf(buf, _count, fmt, args);
    }

    int __cdecl
    snprintf(
        char * const buf,
        size_t const count,
        char const * const fmt,
        ...)
    {
        va_list args;
        va_start(args, fmt);
        auto const ret = _vsnprintf(buf, count, fmt, args);
        va_end(args);
        return ret;
    }

    __declspec(dllimport)
    extern int __cdecl
    _fileno(FILE *stream);

    int __cdecl
    fileno(FILE *stream) {
        return _fileno(stream);
    }

    __declspec(dllimport)
    extern FILE *
    fopen(char const *filename, char const *mode);

    errno_t
    _msvcrt_support_fopen_s(
        FILE **file,
        char const * const filename,
        char const * const mode)
    {
        *file = fopen(filename, mode);
        return _errno;
    }
    #pragma comment(linker, "/alternatename:fopen_s=_msvcrt_support_fopen_s")
}
#endif // defined(MSVCRT_SUPPORT_ENABLE_C_STDIO)

// #Section: CRT emulations of POSIX-style IO functions ------------------------
#if defined(MSVCRT_SUPPORT_ENABLE_POSIX_IO)
extern "C" {
    __declspec(dllimport)
    extern long __cdecl
    _lseek(int fd, long offset, int origin);

    __declspec(dllimport)
    extern int __cdecl
    _open(const char *filename, int oflag, int pmode);

    __declspec(dllimport)
    extern int __cdecl
    _close(int fd);

    __declspec(dllimport)
    extern int __cdecl
    _read(int fd, void *buffer, unsigned int count);

    __declspec(dllimport)
    extern int __cdecl
    _write(int fd, void const *buffer, unsigned int count);

    int __cdecl
    open(const char *filename, int oflag, int pmode) {
        return _open(filename, oflag, pmode);
    }

    int __cdecl
    close(int fd) {
        return _close(fd);
    }

    long __cdecl
    lseek(int fd, long offset, int origin) {
        return _lseek(fd, offset, origin);
    }

    int __cdecl
    read(int fd, void *buffer, unsigned int count) {
        return _read(fd, buffer, count);
    }

    int __cdecl
    write(int fd, void const *buffer, unsigned int count) {
        return _write(fd, buffer, count);
    }
}
#endif

// #Section: COM support -------------------------------------------------------
#if 1
using HRESULT = long;

void __stdcall
_com_issue_error(HRESULT) {
    __fastfail(1);
}
#endif

// #Section: Concurrency control stuff used by the MS C++ stdlib headers -------
//
// This is necessary to build against the Microsoft locale/iostream/etc.
// headers. The MS C++ stdlib presumes that there are set of globally-accessible
// locks accessible via these strangely-named "_Lockit" functions.
#if defined(MSVCRT_SUPPORT_ENABLE_LOCKIT)

namespace { namespace mtx {
    using namespace msvcrt_support;

    static long refs = -1;
    static mtx::mtx_t mutex;

    struct lock_init_t {
        lock_init_t() {
            auto const new_ = _InterlockedIncrement(&refs);
            if (new_ == 0) {
                mtx_init(mutex);
            }
        }

        ~lock_init_t() {
            auto const new_ = _InterlockedDecrement(&refs);
            if (new_ < 0) {
                mtx_uninit(mutex);
            }
        }
    };

    static lock_init_t lock_init;
} }

namespace std {
    struct _Lockit {
        int lock_type;

        __thiscall
        _Lockit();

        explicit __thiscall
        _Lockit(int);

        __thiscall
        ~_Lockit();
    };

    __thiscall
    _Lockit::_Lockit() : lock_type(0) {
        mtx::mtx_lock(mtx::mutex);
    }

    __thiscall
    _Lockit::_Lockit(int const type) : lock_type(type) {
        mtx::mtx_lock(mtx::mutex);
    }

    __thiscall
    _Lockit::~_Lockit() {
        mtx::mtx_unlock(mtx::mutex);
    }
}
#endif // defined(MSVCRT_SUPPORT_ENABLE_LOCKIT)

// #Section: C++ thread-safe statics -------------------------------------------
//
// This is necessary to allow so-called "thread-safe statics" to be implemented.
// Might be removed later. Haven't needed it in the real world.
#if MSVCRT_SUPPORT_ENABLE_THREAD_SAFE_STATICS // thread-safe statics
namespace { namespace tss {
    using namespace msvcrt_support;

    static int const Uninitialized = 0;
    static int const Initializing = -1;

    static mtx_t mutex;
    static condvar_t condvar;
} }

extern "C" {

    int _Init_global_epoch = INT_MIN;
    __declspec(thread) int _Init_thread_epoch = INT_MIN;

    void __cdecl
    _Init_thread_lock() {
        tss::mtx_lock(tss::mutex);
    }

    void __cdecl
    _Init_thread_unlock() {
        tss::mtx_unlock(tss::mutex);
    }

    bool __cdecl
    _Init_thread_wait(DWORD const timeout) {
        return tss::condvar_wait_timeout(tss::condvar, tss::mutex, timeout);
    }

    void __cdecl
    _Init_thread_notify() {
        tss::condvar_notify_all(tss::condvar);
    }

    void __cdecl
    _Init_thread_header(int * const status) {
        _Init_thread_lock();
        if (*status == tss::Uninitialized) {
            *status = tss::Initializing;
        } else {
            while (*status == tss::Initializing) {
                _Init_thread_wait(0);
                if (*status == tss::Uninitialized) {
                    *status = tss::Initializing;
                    _Init_thread_unlock();
                    return;
                }
            }
            _Init_thread_epoch = _Init_global_epoch;
        }
        _Init_thread_unlock();
    }

    void __cdecl
    _Init_thread_abort(int * const) {
        // Only called when an initializer fails due to a C++ exception. C++
        // exceptions are not supported.
        __fastfail(1);
    }

    void __cdecl
    _Init_thread_footer(int * const status) {
        _Init_thread_lock();
        ++_Init_global_epoch;
        *status = _Init_global_epoch;
        _Init_thread_epoch = _Init_global_epoch;
        _Init_thread_unlock();
        _Init_thread_notify();
    }

    void __cdecl
    thread_safe_statics_uninit() {
        mtx_uninit(tss::mutex);
    }

    int __cdecl
    thread_safe_statics_init() {
        mtx_init(tss::mutex); // TODO: Explicitly initialize with a spin count
        condvar_init(tss::condvar);

        // TODO: Clean the mutex up when this module is unloaded.
        //atexit(thread_safe_statics_uninit);
        return 0;
    }

    namespace {
        using tss_init_t = int (__cdecl *) ();
    }

    // This marks `thread_safe_statics_init` as a function that should be run
    // during the initialization of the CRT.
    #pragma section(".CRT$XIC", long, read)
    __declspec(allocate(".CRT$XIC")) tss_init_t __scrt_initialize_tss_var = thread_safe_statics_init;
}
#endif // MSVCRT_SUPPORT_ENABLE_THREAD_SAFE_STATICS

// #Section: C++ operator new/delete support -----------------------------------
#if 1
namespace std {
    struct nothrow_t { };

    extern nothrow_t const nothrow;

    const nothrow_t nothrow { };
}
#endif

// #Section: MS C++ stdlib debug trap handlers ---------------------------------
#if defined(MSVCRT_SUPPORT_ENABLE_CRT_DEBUG)
namespace std {
    void __cdecl
    _Xbad_alloc() {
        __fastfail(1);
    }

    void __cdecl
    _Xinvalid_argument(char const * /* msg */) {
        __fastfail(1);
    }

    void __cdecl
    _Xlength_error(char const * /* msg */) {
        __fastfail(1);
    }

    void __cdecl
    _Xout_of_range(char const * /* msg */) {
        __fastfail(1);
    }

    void __cdecl
    _Xoverflow_error(char const * /* msg */) {
        __fastfail(1);
    }

    void __cdecl
    _Xruntime_error(char const * /* msg */) {
        __fastfail(1);
    }

    void __cdecl
    _Xbad_function_call() {
        __fastfail(1);
    }

    void __cdecl
    _Debug_message(wchar_t const *, wchar_t const *, unsigned int) {
        __fastfail(1);
    }
}
#endif // defined(MSVCRT_SUPPORT_ENABLE_CRT_DEBUG)

// #Section: Timezone support --------------------------------------------------
//
// TODO: These need complete implementations and will probably be backed with
// Win32 API calls.
//
// These are stubbed out for now: Qt needs these symbols to build, but I'm not
// yet sure what parts of Qt really depend on the output of these functions.
#if 1
extern "C" {
    errno_t __cdecl
    _get_timezone(long *tz) {
        *tz = 0;
        return 0;
    }

    errno_t __cdecl
    _get_tzname(size_t *written, char *name, size_t namesz, int) {
        if (namesz >= 2) {
            name[0] = 'a';
            name[1] = 0;
            *written = 2;
        } else if (namesz == 1) {
            name[0] = 0;
            *written = 1;
        } else {
            *written = 0;
        }
        return 0;
    }
}
#endif

// #Section: Locale support ----------------------------------------------------
//
// TODO: These need complete implementations and will probably be backed with
// Win32 API calls.
//
// These are stubbed out for now: Qt needs these symbols to build, but I'm not
// yet sure what parts of Qt really depend on the output of these functions.
#if 1
extern "C" {
    struct __crt_locale_pointers;

    using _locale_t = __crt_locale_pointers *;

    void __cdecl
    _free_locale(_locale_t) {
    }

    _locale_t
    _create_locale(int /* _category */, const char * /* _locale */) {
        return nullptr;
    }

    struct mbstate_t;

    struct _Cvtvec {
        unsigned int _Page;
        unsigned int _Mbcurmax;
        int _Isclocale;
        unsigned char _Isleadbyte[32];
    };

    int __cdecl
    _Mbrtowc(
        wchar_t *out,
        char const *src,
        size_t srcsz,
        mbstate_t *,
        _Cvtvec const *)
    {
        size_t i = 0;
        auto in = src;
        while (*in != 0 && i < srcsz) {
            out[i] = *in;
            ++i;
            ++in;
        }
        return static_cast<int>(i);
    }

    _Cvtvec __cdecl
    _Getcvt() {
        return { };
    }

}

namespace std {
    struct _Locinfo {
        _Cvtvec
        _Getcvt() const;

        unsigned short const *
        _W_Getdays() const;

        unsigned short const *
        _W_Getmonths() const;

        char const *
        _Getdays() const;

        char const *
        _Getmonths() const;
    };

    _Cvtvec
    _Locinfo::_Getcvt() const {
        return { };
    }

    static unsigned short const UshortArr[] = { 0 };

    unsigned short const *
    _Locinfo::_W_Getdays() const {
        return UshortArr;
    }

    unsigned short const *
    _Locinfo::_W_Getmonths() const {
        return UshortArr;
    }

    static char const UcharArr[] = { 0 };

    char const *
    _Locinfo::_Getdays() const {
        return UcharArr;
    }

    char const *
    _Locinfo::_Getmonths() const {
        return UcharArr;
    }
}
#endif

// #Section: Support for std::type_info ----------------------------------------
//
// This provides basic support for manipulating instances of std::type_info
// that are generated by MSVC2015. These are static, and their presence does
// not imply C++ RTTI. This is needed for std::shared_ptr, which depends
// on std::type_info.
#if 1
extern "C" {
    struct __std_type_info_data {
        char const * undecorated;
        char const decorated[1];
    };

    __declspec(dllimport)
    extern int __cdecl
    strcmp(char const * x, char const * y);

    int __cdecl
    __std_type_info_compare(
        __std_type_info_data const * const x,
        __std_type_info_data const * const y)
    {
        if (x == y) return 0;
        return strcmp(x->decorated + 1, y->decorated + 1);
    }
}
#endif

// #Section: Trap-on-failure support for C++ exceptions ------------------------
//
// This provides a few symbols that you might encounter when using the MS C++
// stdlib without the MSVC2015 runtime library.
#if 1
namespace stdext {
    class exception;
}
static void __cdecl
raise_handler(const stdext::exception &) {
    __fastfail(1);
}
using raise_handler_t = void (__cdecl *) (const stdext::exception &);
namespace std {
    extern raise_handler_t _Raise_handler;
}
raise_handler_t std::_Raise_handler = raise_handler;
#endif

// #Section: unistd.h stubs ----------------------------------------------------

extern "C" {
    __declspec(dllimport)
    extern int __cdecl
    _access(char const *filename, int accessmode);

    int __cdecl
    access(char const *filename, int accessmode) {
        return _access(filename, accessmode);
    }
}

// #Section: VS2019 vectorized std::reverse stubs ------------------------------

template <typename t>
static void __forceinline
rev(t *begin, t *end) {
    for (; begin != end && begin != --end; ++begin) {
        auto const temp = *begin;
        *begin = *end;
        *end = temp;
    }
}

extern "C" {
    __declspec(noalias) void __cdecl
    __std_reverse_trivially_swappable_1(void* begin, void* end) {
        using t = __int8 unsigned;
        rev(static_cast<t *>(begin), static_cast<t *>(end));
    }

    __declspec(noalias) void __cdecl
    __std_reverse_trivially_swappable_2(void* begin, void* end) {
        using t = __int16 unsigned;
        rev(static_cast<t *>(begin), static_cast<t *>(end));
    }

    __declspec(noalias) void __cdecl
    __std_reverse_trivially_swappable_4(void* begin, void* end) {
        using t = __int32 unsigned;
        rev(static_cast<t *>(begin), static_cast<t *>(end));
    }

    __declspec(noalias) void __cdecl
    __std_reverse_trivially_swappable_8(void* begin, void* end) {
        using t = __int64 unsigned;
        rev(static_cast<t *>(begin), static_cast<t *>(end));
    }
}

// #Section: VS2022 vectorized stdlib stubs ------------------------------------

template <typename t>
static __forceinline void *
find(t * begin, t * const end, t const pred_val) {
    for (; begin != end; ++begin) {
        if (*begin == pred_val) break;
    }
    return begin;
}

extern "C" {
    void * __cdecl
    __std_find_trivial_1(void *begin, void *end, unsigned __int8 pred_val) {
        return find(static_cast<unsigned __int8 *>(begin), static_cast<unsigned __int8 *>(end), pred_val);
    }

    void * __cdecl
    __std_find_trivial_2(void *begin, void *end, unsigned __int32 pred_val) {
        return find(static_cast<unsigned __int32 *>(begin), static_cast<unsigned __int32 *>(end), pred_val);
    }

    void * __cdecl
    __std_find_trivial_4(void *begin, void *end, unsigned __int32 pred_val) {
        return find(static_cast<unsigned __int32 *>(begin), static_cast<unsigned __int32 *>(end), pred_val);
    }

    void * __cdecl
    __std_find_trivial_8(void *begin, void *end, unsigned __int64 pred_val) {
        return find(static_cast<unsigned __int64 *>(begin), static_cast<unsigned __int64 *>(end), pred_val);
    }
}

template <typename t>
static __forceinline size_t
count(t *begin, t * const end, t const pred_val) {
    size_t ret = 0;
    for (; begin != end; ++begin) {
        if (*begin == pred_val) ++ret;
    }
    return ret;
}

extern "C" {
    void * __cdecl
    __std_count_trivial_1(void *begin, void *end, unsigned __int8 pred_val) {
        return find(static_cast<unsigned __int8 *>(begin), static_cast<unsigned __int8 *>(end), pred_val);
    }

    void * __cdecl
    __std_count_trivial_2(void *begin, void *end, unsigned __int16 pred_val) {
        return find(static_cast<unsigned __int16 *>(begin), static_cast<unsigned __int16 *>(end), pred_val);
    }

    void * __cdecl
    __std_count_trivial_4(void *begin, void *end, unsigned __int32 pred_val) {
        return find(static_cast<unsigned __int32 *>(begin), static_cast<unsigned __int32 *>(end), pred_val);
    }

    void * __cdecl
    __std_count_trivial_8(void *begin, void *end, unsigned __int64 pred_val) {
        return find(static_cast<unsigned __int64 *>(begin), static_cast<unsigned __int64 *>(end), pred_val);
    }
}

// #Section: std::error_category support functions -----------------------------

namespace std {
    char const *
    _Syserror_map(int) {
        return "<unknown error>";
    }
}

// #Section: _localtime64_s ----------------------------------------------------

// XXXih: HACK: Do a real implementation of localtime64/mktime64
struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

using __time32_t = long;
using time_t = long long;
using __time64_t = __int64;

extern "C" {
    __declspec(dllimport)
    extern tm * __cdecl
    localtime(__time32_t const * const t);

    __declspec(dllimport)
    extern __time32_t __cdecl
    mktime(tm * const t);

    __declspec(noinline)
    tm * __cdecl
    fake_localtime64(__time64_t const * const t) {
        auto const t_ = static_cast<__time32_t>(*t);
        return localtime(&t_);
    }

    __time64_t __cdecl
    fake_mktime64(tm * const t) {
        return mktime(t);
    }

    #if defined(_M_IX86)
    __time64_t __cdecl
    _mktime64(tm * const t) {
        return mktime(t);
    }
    #endif

    extern "C"
    errno_t __cdecl
    _localtime64_s(tm * tm, __time64_t const * time) {
        if (tm == nullptr || time == nullptr) {
            return 0;
        }
        *tm = *fake_localtime64(time);
        return 0;
    }

    #if defined(_M_IX86)
    __declspec(dllimport)
    extern struct tm *
    gmtime(__time32_t const * const source_time);

    struct tm *
    _gmtime64(__time64_t const * const source_time) {
        auto const truncated_time = static_cast<__time32_t>(*source_time);
        return gmtime(&truncated_time);
    }
    #endif
}

// #Section: platform-utf-16 assertions ----------------------------------------

extern "C" {
    __declspec(noreturn)
    void __cdecl
    _wassert(
        wchar_t const * const message,
        wchar_t const * const filename,
        unsigned const line)
    {
        __debugbreak();
        __fastfail(1);
    }
}

// #Section: nothrow new -------------------------------------------------------
#if 1
extern "C" {
    __declspec(dllimport)
    extern void * __cdecl
    malloc(size_t);
}

extern "C" {
    void *
    _msvcrt_support_nothrow_new(size_t const sz, void const * /* std::nothrow_t const & */) {
        return ::malloc(sz);
    }
    #if defined(_M_AMD64)
        // void * __cdecl operator new(unsigned int,struct std::nothrow_t const &)
        #pragma comment(linker, "/alternatename:??2@YAPAXIABUnothrow_t@std@@@Z=_msvcrt_support_nothrow_new")

        // void * __ptr64 __cdecl operator new[](unsigned __int64,struct std::nothrow_t const & __ptr64)
        #pragma comment(linker, "/alternatename:??_U@YAPEAX_KAEBUnothrow_t@std@@@Z=_msvcrt_support_nothrow_new")

        // void * __ptr64 __cdecl operator new(unsigned __int64,struct std::nothrow_t const & __ptr64)
        #pragma comment(linker, "/alternatename:??2@YAPEAX_KAEBUnothrow_t@std@@@Z=_msvcrt_support_nothrow_new")
    #endif
}
#endif

#if 1
#if defined(_M_AMD64) || defined(_M_IX86)
extern "C" {
    #if defined(_CRTALLOC)
        #undef _CRTALLOC
    #endif
    #define _CRTALLOC(x) __declspec(allocate(x))

    #pragma section(".CRT$XIA", long, read)
    #pragma section(".CRT$XIZ", long, read)
    #pragma section(".CRT$XCA", long, read)
    #pragma section(".CRT$XCZ", long, read)

    using _PVFV = void (__cdecl *) ();
    using _PIFV = int (__cdecl *) ();

    #pragma comment(linker, "/merge:.CRT=.rdata")
    _CRTALLOC(".CRT$XIA") _PVFV __xi_a[] = { nullptr };
    _CRTALLOC(".CRT$XIZ") _PVFV __xi_z[] = { nullptr };
    _CRTALLOC(".CRT$XCA") _PVFV __xc_a[] = { nullptr };
    _CRTALLOC(".CRT$XCZ") _PVFV __xc_z[] = { nullptr };
}
#endif
#endif

// #Section: chkstk support ----------------------------------------------------
#if 1
#if defined(_M_AMD64)
    #pragma comment(linker, "/alternatename:__chkstk=_msvcrt_support_chkstk")
#elif defined(_M_IX86)
    // The following function is derived from LLVM, which is licensed under the
    // "Apache-2.0 WITH LLVM-exception" license. The full license text is too
    // large to include here, but the summary is that you do not need to
    // provide prominent attribution to the LLVM project when distributing
    // distributing object code that incorporates this function.
    extern "C"
    void __declspec(naked)
    msvcrt_support_chkstk_alloca() {
        __asm {
            push   ecx
            cmp    eax, 1000h
            lea    ecx, [esp] + 8   // esp before calling this routine -> ecx
            jb     label1

            label2:
            sub    ecx, 1000h
            test   [ecx], ecx
            sub    eax, 1000h
            cmp    eax, 1000h
            ja     label2

            label1:
            sub    ecx, eax
            test   [ecx], ecx

            lea    eax, [esp] + 4   // load pointer to the return address into eax
            mov    esp, ecx         // install the new top of stack pointer into esp
            mov    ecx, [eax] - 4   // restore ecx
            push   [eax]            // push return address onto the stack
            sub    eax, esp         // restore the original value in eax
            ret
        }
    }
    #pragma comment(linker, "/alternatename:__chkstk=_msvcrt_support_chkstk_alloca")
#endif
#endif

#endif // defined(_MSC_VER)

// #Section: type_info symbols -------------------------------------------------
#if 1
class type_info
{
public:
    virtual ~type_info();
};

type_info::~type_info() { }

type_info _dummy_type_info_;
#endif

// #Section: i386 support ------------------------------------------------------

// The code included in this `extern "C"` block is derived from revision
// 5c894fec85b9 of SDL's src/stdlib/SDL_stdlib.c. This code is licensed under
// the following license:
//
// Simple DirectMedia Layer
// Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
#if defined(_M_IX86)
extern "C" {
    void __declspec(naked)
    _msvcrt_support_ftol() {
        __asm {
            push        ebp
            mov         ebp,esp
            sub         esp,20h
            and         esp,0FFFFFFF0h
            fld         st(0)
            fst         dword ptr [esp+18h]
            fistp       qword ptr [esp+10h]
            fild        qword ptr [esp+10h]
            mov         edx,dword ptr [esp+18h]
            mov         eax,dword ptr [esp+10h]
            test        eax,eax
            je          integer_QnaN_or_zero
    arg_is_not_integer_QnaN:
            fsubp       st(1),st
            test        edx,edx
            jns         positive
            fstp        dword ptr [esp]
            mov         ecx,dword ptr [esp]
            xor         ecx,80000000h
            add         ecx,7FFFFFFFh
            adc         eax,0
            mov         edx,dword ptr [esp+14h]
            adc         edx,0
            jmp         localexit
    positive:
            fstp        dword ptr [esp]
            mov         ecx,dword ptr [esp]
            add         ecx,7FFFFFFFh
            sbb         eax,0
            mov         edx,dword ptr [esp+14h]
            sbb         edx,0
            jmp         localexit
    integer_QnaN_or_zero:
            mov         edx,dword ptr [esp+14h]
            test        edx,7FFFFFFFh
            jne         arg_is_not_integer_QnaN
            fstp        dword ptr [esp+18h]
            fstp        dword ptr [esp+18h]
    localexit:
            leave
            ret
        }
    }
    #pragma comment(linker, "/alternatename:__ftol2_sse=__msvcrt_support_ftol")
    #pragma comment(linker, "/alternatename:__ftol2=__msvcrt_support_ftol")

    void __declspec(naked)
    _msvcrt_support_allmul() {
        __asm {
            mov         eax, dword ptr[esp+8]
            mov         ecx, dword ptr[esp+10h]
            or          ecx, eax
            mov         ecx, dword ptr[esp+0Ch]
            jne         hard
            mov         eax, dword ptr[esp+4]
            mul         ecx
            ret         10h
    hard:
            push        ebx
            mul         ecx
            mov         ebx, eax
            mov         eax, dword ptr[esp+8]
            mul         dword ptr[esp+14h]
            add         ebx, eax
            mov         eax, dword ptr[esp+8]
            mul         ecx
            add         edx, ebx
            pop         ebx
            ret         10h
        }
    }
    #pragma comment(linker, "/alternatename:__allmul=__msvcrt_support_allmul")

    void __declspec(naked)
    _msvcrt_support_alldiv() {
        __asm {
            push        edi
            push        esi
            push        ebx
            xor         edi,edi
            mov         eax,dword ptr [esp+14h]
            or          eax,eax
            jge         L1
            inc         edi
            mov         edx,dword ptr [esp+10h]
            neg         eax
            neg         edx
            sbb         eax,0
            mov         dword ptr [esp+14h],eax
            mov         dword ptr [esp+10h],edx
    L1:
            mov         eax,dword ptr [esp+1Ch]
            or          eax,eax
            jge         L2
            inc         edi
            mov         edx,dword ptr [esp+18h]
            neg         eax
            neg         edx
            sbb         eax,0
            mov         dword ptr [esp+1Ch],eax
            mov         dword ptr [esp+18h],edx
    L2:
            or          eax,eax
            jne         L3
            mov         ecx,dword ptr [esp+18h]
            mov         eax,dword ptr [esp+14h]
            xor         edx,edx
            div         ecx
            mov         ebx,eax
            mov         eax,dword ptr [esp+10h]
            div         ecx
            mov         edx,ebx
            jmp         L4
    L3:
            mov         ebx,eax
            mov         ecx,dword ptr [esp+18h]
            mov         edx,dword ptr [esp+14h]
            mov         eax,dword ptr [esp+10h]
    L5:
            shr         ebx,1
            rcr         ecx,1
            shr         edx,1
            rcr         eax,1
            or          ebx,ebx
            jne         L5
            div         ecx
            mov         esi,eax
            mul         dword ptr [esp+1Ch]
            mov         ecx,eax
            mov         eax,dword ptr [esp+18h]
            mul         esi
            add         edx,ecx
            jb          L6
            cmp         edx,dword ptr [esp+14h]
            ja          L6
            jb          L7
            cmp         eax,dword ptr [esp+10h]
            jbe         L7
    L6:
            dec         esi
    L7:
            xor         edx,edx
            mov         eax,esi
    L4:
            dec         edi
            jne         L8
            neg         edx
            neg         eax
            sbb         edx,0
    L8:
            pop         ebx
            pop         esi
            pop         edi
            ret         10h
        }
    }
    #pragma comment(linker, "/alternatename:__alldiv=__msvcrt_support_alldiv")

    void __declspec(naked)
    _msvcrt_support_aulldiv() {
        __asm {
            push        ebx
            push        esi
            mov         eax,dword ptr [esp+18h]
            or          eax,eax
            jne         L1
            mov         ecx,dword ptr [esp+14h]
            mov         eax,dword ptr [esp+10h]
            xor         edx,edx
            div         ecx
            mov         ebx,eax
            mov         eax,dword ptr [esp+0Ch]
            div         ecx
            mov         edx,ebx
            jmp         L2
    L1:
            mov         ecx,eax
            mov         ebx,dword ptr [esp+14h]
            mov         edx,dword ptr [esp+10h]
            mov         eax,dword ptr [esp+0Ch]
    L3:
            shr         ecx,1
            rcr         ebx,1
            shr         edx,1
            rcr         eax,1
            or          ecx,ecx
            jne         L3
            div         ebx
            mov         esi,eax
            mul         dword ptr [esp+18h]
            mov         ecx,eax
            mov         eax,dword ptr [esp+14h]
            mul         esi
            add         edx,ecx
            jb          L4
            cmp         edx,dword ptr [esp+10h]
            ja          L4
            jb          L5
            cmp         eax,dword ptr [esp+0Ch]
            jbe         L5
    L4:
            dec         esi
    L5:
            xor         edx,edx
            mov         eax,esi
    L2:
            pop         esi
            pop         ebx
            ret         10h
        }
    }
    #pragma comment(linker, "/alternatename:__aulldiv=__msvcrt_support_aulldiv")

    void __declspec(naked)
    _allrem() {
        __asm {
            push        ebx
            push        edi
            xor         edi,edi
            mov         eax,dword ptr [esp+10h]
            or          eax,eax
            jge         L1
            inc         edi
            mov         edx,dword ptr [esp+0Ch]
            neg         eax
            neg         edx
            sbb         eax,0
            mov         dword ptr [esp+10h],eax
            mov         dword ptr [esp+0Ch],edx
    L1:
            mov         eax,dword ptr [esp+18h]
            or          eax,eax
            jge         L2
            mov         edx,dword ptr [esp+14h]
            neg         eax
            neg         edx
            sbb         eax,0
            mov         dword ptr [esp+18h],eax
            mov         dword ptr [esp+14h],edx
    L2:
            or          eax,eax
            jne         L3
            mov         ecx,dword ptr [esp+14h]
            mov         eax,dword ptr [esp+10h]
            xor         edx,edx
            div         ecx
            mov         eax,dword ptr [esp+0Ch]
            div         ecx
            mov         eax,edx
            xor         edx,edx
            dec         edi
            jns         L4
            jmp         L8
    L3:
            mov         ebx,eax
            mov         ecx,dword ptr [esp+14h]
            mov         edx,dword ptr [esp+10h]
            mov         eax,dword ptr [esp+0Ch]
    L5:
            shr         ebx,1
            rcr         ecx,1
            shr         edx,1
            rcr         eax,1
            or          ebx,ebx
            jne         L5
            div         ecx
            mov         ecx,eax
            mul         dword ptr [esp+18h]
            xchg        eax,ecx
            mul         dword ptr [esp+14h]
            add         edx,ecx
            jb          L6
            cmp         edx,dword ptr [esp+10h]
            ja          L6
            jb          L7
            cmp         eax,dword ptr [esp+0Ch]
            jbe         L7
    L6:
            sub         eax,dword ptr [esp+14h]
            sbb         edx,dword ptr [esp+18h]
    L7:
            sub         eax,dword ptr [esp+0Ch]
            sbb         edx,dword ptr [esp+10h]
            dec         edi
            jns         L8
    L4:
            neg         edx
            neg         eax
            sbb         edx,0
    L8:
            pop         edi
            pop         ebx
            ret         10h
        }
    }
    #pragma comment(linker, "/alternatename:__allrem=__msvcrt_support_allrem")

    void __declspec(naked)
    _msvcrt_support_aullrem() {
        __asm {
            push        ebx
            mov         eax,dword ptr [esp+14h]
            or          eax,eax
            jne         L1
            mov         ecx,dword ptr [esp+10h]
            mov         eax,dword ptr [esp+0Ch]
            xor         edx,edx
            div         ecx
            mov         eax,dword ptr [esp+8]
            div         ecx
            mov         eax,edx
            xor         edx,edx
            jmp         L2
    L1:
            mov         ecx,eax
            mov         ebx,dword ptr [esp+10h]
            mov         edx,dword ptr [esp+0Ch]
            mov         eax,dword ptr [esp+8]
    L3:
            shr         ecx,1
            rcr         ebx,1
            shr         edx,1
            rcr         eax,1
            or          ecx,ecx
            jne         L3
            div         ebx
            mov         ecx,eax
            mul         dword ptr [esp+14h]
            xchg        eax,ecx
            mul         dword ptr [esp+10h]
            add         edx,ecx
            jb          L4
            cmp         edx,dword ptr [esp+0Ch]
            ja          L4
            jb          L5
            cmp         eax,dword ptr [esp+8]
            jbe         L5
    L4:
            sub         eax,dword ptr [esp+10h]
            sbb         edx,dword ptr [esp+14h]
    L5:
            sub         eax,dword ptr [esp+8]
            sbb         edx,dword ptr [esp+0Ch]
            neg         edx
            neg         eax
            sbb         edx,0
    L2:
            pop         ebx
            ret         10h
        }
    }
    #pragma comment(linker, "/alternatename:__aullrem=__msvcrt_support_aullrem")

    void __declspec(naked)
    _msvcrt_support_alldvrm() {
        __asm {
            push        edi
            push        esi
            push        ebp
            xor         edi,edi
            xor         ebp,ebp
            mov         eax,dword ptr [esp+14h]
            or          eax,eax
            jge         L1
            inc         edi
            inc         ebp
            mov         edx,dword ptr [esp+10h]
            neg         eax
            neg         edx
            sbb         eax,0
            mov         dword ptr [esp+14h],eax
            mov         dword ptr [esp+10h],edx
    L1:
            mov         eax,dword ptr [esp+1Ch]
            or          eax,eax
            jge         L2
            inc         edi
            mov         edx,dword ptr [esp+18h]
            neg         eax
            neg         edx
            sbb         eax,0
            mov         dword ptr [esp+1Ch],eax
            mov         dword ptr [esp+18h],edx
    L2:
            or          eax,eax
            jne         L3
            mov         ecx,dword ptr [esp+18h]
            mov         eax,dword ptr [esp+14h]
            xor         edx,edx
            div         ecx
            mov         ebx,eax
            mov         eax,dword ptr [esp+10h]
            div         ecx
            mov         esi,eax
            mov         eax,ebx
            mul         dword ptr [esp+18h]
            mov         ecx,eax
            mov         eax,esi
            mul         dword ptr [esp+18h]
            add         edx,ecx
            jmp         L4
    L3:
            mov         ebx,eax
            mov         ecx,dword ptr [esp+18h]
            mov         edx,dword ptr [esp+14h]
            mov         eax,dword ptr [esp+10h]
    L5:
            shr         ebx,1
            rcr         ecx,1
            shr         edx,1
            rcr         eax,1
            or          ebx,ebx
            jne         L5
            div         ecx
            mov         esi,eax
            mul         dword ptr [esp+1Ch]
            mov         ecx,eax
            mov         eax,dword ptr [esp+18h]
            mul         esi
            add         edx,ecx
            jb          L6
            cmp         edx,dword ptr [esp+14h]
            ja          L6
            jb          L7
            cmp         eax,dword ptr [esp+10h]
            jbe         L7
    L6:
            dec         esi
            sub         eax,dword ptr [esp+18h]
            sbb         edx,dword ptr [esp+1Ch]
    L7:
            xor         ebx,ebx
    L4:
            sub         eax,dword ptr [esp+10h]
            sbb         edx,dword ptr [esp+14h]
            dec         ebp
            jns         L9
            neg         edx
            neg         eax
            sbb         edx,0
    L9:
            mov         ecx,edx
            mov         edx,ebx
            mov         ebx,ecx
            mov         ecx,eax
            mov         eax,esi
            dec         edi
            jne         L8
            neg         edx
            neg         eax
            sbb         edx,0
    L8:
            pop         ebp
            pop         esi
            pop         edi
            ret         10h
        }
    }
    #pragma comment(linker, "/alternatename:__alldvrm=__msvcrt_support_alldvrm")

    void __declspec(naked)
    _aulldvrm() {
        __asm {
            push        esi
            mov         eax,dword ptr [esp+14h]
            or          eax,eax
            jne         L1
            mov         ecx,dword ptr [esp+10h]
            mov         eax,dword ptr [esp+0Ch]
            xor         edx,edx
            div         ecx
            mov         ebx,eax
            mov         eax,dword ptr [esp+8]
            div         ecx
            mov         esi,eax
            mov         eax,ebx
            mul         dword ptr [esp+10h]
            mov         ecx,eax
            mov         eax,esi
            mul         dword ptr [esp+10h]
            add         edx,ecx
            jmp         L2
    L1:
            mov         ecx,eax
            mov         ebx,dword ptr [esp+10h]
            mov         edx,dword ptr [esp+0Ch]
            mov         eax,dword ptr [esp+8]
    L3:
            shr         ecx,1
            rcr         ebx,1
            shr         edx,1
            rcr         eax,1
            or          ecx,ecx
            jne         L3
            div         ebx
            mov         esi,eax
            mul         dword ptr [esp+14h]
            mov         ecx,eax
            mov         eax,dword ptr [esp+10h]
            mul         esi
            add         edx,ecx
            jb          L4
            cmp         edx,dword ptr [esp+0Ch]
            ja          L4
            jb          L5
            cmp         eax,dword ptr [esp+8]
            jbe         L5
    L4:
            dec         esi
            sub         eax,dword ptr [esp+10h]
            sbb         edx,dword ptr [esp+14h]
    L5:
            xor         ebx,ebx
    L2:
            sub         eax,dword ptr [esp+8]
            sbb         edx,dword ptr [esp+0Ch]
            neg         edx
            neg         eax
            sbb         edx,0
            mov         ecx,edx
            mov         edx,ebx
            mov         ebx,ecx
            mov         ecx,eax
            mov         eax,esi
            pop         esi
            ret         10h
        }
    }
    #pragma comment(linker, "/alternatename:__aulldvrm=__msvcrt_support_aulldvrm")

    void __declspec(naked)
    _msvcrt_support_allshl() {
        __asm {
            cmp         cl,40h
            jae         RETZERO
            cmp         cl,20h
            jae         MORE32
            shld        edx,eax,cl
            shl         eax,cl
            ret
    MORE32:
            mov         edx,eax
            xor         eax,eax
            and         cl,1Fh
            shl         edx,cl
            ret
    RETZERO:
            xor         eax,eax
            xor         edx,edx
            ret
        }
    }
    #pragma comment(linker, "/alternatename:__allshl=__msvcrt_support_allshl")

    void __declspec(naked)
    _msvcrt_support_allshr() {
        __asm {
            cmp         cl,40h
            jae         RETZERO
            cmp         cl,20h
            jae         MORE32
            shrd        eax,edx,cl
            sar         edx,cl
            ret
    MORE32:
            mov         eax,edx
            xor         edx,edx
            and         cl,1Fh
            sar         eax,cl
            ret
    RETZERO:
            xor         eax,eax
            xor         edx,edx
            ret
        }
    }
    #pragma comment(linker, "/alternatename:__allshr=__msvcrt_support_allshr")

    void __declspec(naked)
    _aullshr() {
        __asm {
            cmp         cl,40h
            jae         RETZERO
            cmp         cl,20h
            jae         MORE32
            shrd        eax,edx,cl
            shr         edx,cl
            ret
    MORE32:
            mov         eax,edx
            xor         edx,edx
            and         cl,1Fh
            shr         eax,cl
            ret
    RETZERO:
            xor         eax,eax
            xor         edx,edx
            ret
        }
    }
    #pragma comment(linker, "/alternatename:__aullshr=__msvcrt_support_aullshr")
}
#endif
