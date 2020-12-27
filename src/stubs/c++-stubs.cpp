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

#if defined(__GNUC__) || defined(_MSC_VER)
    #include <new>
#endif

#if defined(__GNUC__)

extern "C" {
    void
    __cxa_pure_virtual() {
        __builtin_trap();
        __builtin_unreachable();
    }
}

namespace std {

std::nothrow_t const nothrow;

void
__throw_bad_alloc() {
    __builtin_trap();
    __builtin_unreachable();
}

void
__throw_length_error(char const *) {
    __builtin_trap();
    __builtin_unreachable();
}

void
__throw_system_error(int) {
    __builtin_trap();
    __builtin_unreachable();
}

void
__throw_bad_function_call() {
    __builtin_trap();
    __builtin_unreachable();
}

}

#endif

#if defined(_MSC_VER)
    #define DLLIMPORT __declspec(dllimport)
#else
    #define DLLIMPORT
#endif

#if defined(__GNUC__) || defined(_MSC_VER)

extern "C" {
    DLLIMPORT
    void *
    malloc(size_t);

    DLLIMPORT
    void
    free(void *);
}

void *
operator new (size_t sz) {
    return malloc(sz);
}

void *
operator new (size_t sz, std::nothrow_t const &) {
    return malloc(sz);
}

void *
operator new[] (size_t sz) {
    return malloc(sz);
}


void *
operator new[] (size_t sz, std::nothrow_t const &) {
    return malloc(sz);
}

void
operator delete[] (void *ptr) {
    free(ptr);
}

void
operator delete (void *ptr) {
    free(ptr);
}

void
operator delete[] (void *ptr, size_t) {
    free(ptr);
}

void
operator delete (void *ptr, size_t) {
    free(ptr);
}

#endif
