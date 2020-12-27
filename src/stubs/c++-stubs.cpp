#if defined(__GNUC__) || defined(_MSC_VER)
    #include <new>
#endif

#include <stddef.h> // size_t

#if defined(_MSC_VER)
    #define DLLIMPORT __declspec(dllimport)
#else
    #define DLLIMPORT
#endif

#if defined(__GNUC__) || defined(_MSC_VER) // (
extern "C" {
    DLLIMPORT
    void *
    malloc(size_t);

    DLLIMPORT
    void
    free(void *);
}
#endif // )

#if defined(__GNUC__)
    #pragma GCC visibility push(hidden) // (
    #define SQT_HIDDEN __attribute__((visibility("hidden")))
#endif


#if defined(__GNUC__) // (

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

void
__throw_bad_array_new_length() {
    __builtin_trap();
    __builtin_unreachable();
}

}

#endif // ) defined(__GNUC__)

#if defined(__GNUC__) || defined(_MSC_VER) // (

void *
operator new (size_t sz);

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

#endif // )

#if defined(__GNUC__)
    #pragma GCC visibility pop // )
#endif

#if defined(__linux__) // (
__asm__(".hidden _ZSt7nothrow");
__asm__(".hidden _ZdaPv");
__asm__(".hidden _ZdaPvm");
__asm__(".hidden _ZdlPv");
__asm__(".hidden _ZdlPvm");
__asm__(".hidden _ZdlPvSt11align_val_t");
__asm__(".hidden _ZdlPvmSt11align_val_t");
__asm__(".hidden _Znam");
__asm__(".hidden _ZnamPv");
__asm__(".hidden _ZnamRKSt9nothrow_t");
__asm__(".hidden _Znwm");
__asm__(".hidden _ZnwmPv");
__asm__(".hidden _ZnwmRKSt9nothrow_t");
#endif // )
