/*
Derived from the MinGW-w64 statically-linked CRT initialization sources, which
are released under the public domain.

This file is similarly released to the public domain.
*/
#if defined(_M_AMD64) || defined(_M_IX86)

#include <windows.h>

#if defined(_CRTALLOC)
    #undef _CRTALLOC
#endif
#define _CRTALLOC(x) __declspec(allocate(x))

extern "C" {

#pragma section(".CRT$XIA", long, read)
#pragma section(".CRT$XIZ", long, read)
#pragma section(".CRT$XCA", long, read)
#pragma section(".CRT$XCZ", long, read)

using _PVFV = void (__cdecl *) ();
using _PIFV = int (__cdecl *) ();

extern _CRTALLOC(".CRT$XIA") _PIFV __xi_a[];
extern _CRTALLOC(".CRT$XIZ") _PIFV __xi_z[];
extern _CRTALLOC(".CRT$XCA") _PVFV __xc_a[];
extern _CRTALLOC(".CRT$XCZ") _PVFV __xc_z[];

static void __declspec(noinline)
run_before_main_init(void const * const begin, void const * const end) {
    auto const _begin = reinterpret_cast<_PVFV const *>(begin);
    auto const _end = reinterpret_cast<_PVFV const *>(end);
    for (auto cur = _begin; cur < _end; ++cur) {
        auto const fn = *cur;
        if (fn != nullptr) fn();
    }
}

int __stdcall
_msvcrt_support_default_WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    ExitProcess(0);
}

BOOL __stdcall
_msvcrt_support_default_DllMain(
    HANDLE hDllHandle,
    DWORD dwReason,
    LPVOID lpReserved)
{
    return TRUE;
}

#if defined(_M_AMD64)
    #pragma comment(linker, "/alternatename:WinMain=_msvcrt_support_default_WinMain")
    #pragma comment(linker, "/alternatename:DllMain=_msvcrt_support_default_DllMain")
#elif defined(_M_IX86)
    #pragma comment(linker, "/alternatename:_WinMain@16=__msvcrt_support_default_WinMain@16")
    #pragma comment(linker, "/alternatename:_DllMain@12=__msvcrt_support_default_DllMain@12")
#endif

extern BOOL __stdcall DllMain (HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved);

#if 1
int __cdecl
WinMainCRTStartup() {
    run_before_main_init(__xi_a, __xi_z);
    run_before_main_init(__xc_a, __xc_z);
    auto const ret = WinMain(nullptr, nullptr, nullptr, 0);
    ExitProcess(ret);
}

int __cdecl
mainCRTStartup() {
    run_before_main_init(__xi_a, __xi_z);
    run_before_main_init(__xc_a, __xc_z);
    auto const ret = WinMain(nullptr, nullptr, nullptr, 0);
    ExitProcess(ret);
}

static int proc_attached = 0;

BOOL __stdcall
_DllMainCRTStartup(HANDLE hdl, DWORD reason, LPVOID reserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        ++proc_attached;
        run_before_main_init(__xi_a, __xi_z);
        run_before_main_init(__xc_a, __xc_z);
        break;
    case DLL_PROCESS_DETACH:
        --proc_attached;
        break;
    default:
        break;
    }
    return DllMain(hdl, reason, reserved);
}
#endif

}

#endif
