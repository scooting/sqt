// #define SQT_WINSTUBS_NO_PREPROC_FORWARDING

#include <initguid.h>

#include "windows-stubs.hpp"
#include "windows-stubs-util.hpp"

#include <atomic>


namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_kernel32();
} }

#define SQT_WINSTUBS_IMPLEMENTATION_kernel32
#define SQT_WINSTUBS_NO_FPTR_DECL
#include "windows-stubs-gen.hpp"
#undef SQT_WINSTUBS_NO_FPTR_DECL

namespace sqt { namespace winapi {
    using NTSTATUS = LONG;

    enum PROCESSINFOCLASS {
        ProcessBasicInformation = 0,
    };

    struct PROCESS_BASIC_INFORMATION {
        PVOID Reserved1;
        uintptr_t PebBaseAddress; // PPEB PebBaseAddress;
        PVOID Reserved2[2];
        ULONG_PTR UniqueProcessId;
        PVOID Reserved3;
    };

    using NtQueryInformationProcess_t = NTSTATUS (WINAPI *)
        ( HANDLE ProcessHandle
        , PROCESSINFOCLASS ProcessInformationClass
        , PVOID ProcessInformation
        , ULONG ProcessInformationLength
        , PULONG ReturnLength
        );

    std::atomic<NtQueryInformationProcess_t> fp_NtQueryInformationProcess;

    static inline bool
    nt_success(NTSTATUS const code) {
        return static_cast<int32_t>(code) >= 0;
    }

    static BOOL WINAPI
    stub_CancelIoEx(
        HANDLE hFile,
        LPOVERLAPPED lpOverlapped)
    {
        return 1;
    }

    static DWORD WINAPI
    stub_GetProcessId(HANDLE hdl) {
        auto fp = fp_NtQueryInformationProcess.load(std::memory_order_relaxed);
        if (SQT_UNLIKELY(fp == nullptr)) {
            auto const ntdll = GetModuleHandleW(L"ntdll.dll");
            if (ntdll == nullptr) goto failed;
            auto const pa = GetProcAddress(ntdll, "NtQueryInformationProcess");
            if (pa == nullptr) goto failed;
            fp = reinterpret_cast<NtQueryInformationProcess_t>(pa);
            fp_NtQueryInformationProcess.store(fp, std::memory_order_relaxed);
        }
        {
            auto info = PROCESS_BASIC_INFORMATION { };
            auto const ret = fp(hdl, ProcessBasicInformation, &info, sizeof(PROCESS_BASIC_INFORMATION), nullptr);
            if (!nt_success(ret)) goto failed;
            return static_cast<DWORD>(info.UniqueProcessId);
        }
        failed:
        return 0;
    }

    static void WINAPI
    stub_GetNativeSystemInfo(LPSYSTEM_INFO info) {
        GetSystemInfo(info);
    }

    static BOOL __stdcall
    stub_CheckRemoteDebuggerPresent(HANDLE, PBOOL debugger_present) {
        *debugger_present = FALSE;
        return TRUE;
    }

    static DWORD __stdcall
    stub_WTSGetActiveConsoleSessionId() {
        return 0xFFFFFFFF;
    }

    static BOOL __stdcall
    stub_GetVolumeNameForVolumeMountPointW(
        LPCTSTR lpszVolumeMountPoint,
        LPTSTR lpszVolumeName,
        DWORD cchBufferLength)
    {
        return 0;
    }

    static BOOL __stdcall
    stub_GetVolumePathNameW(
        LPCTSTR lpszFileName,
        LPTSTR lpszVolumePathName,
        DWORD cchBufferLength)
    {
        return 0;
    }

    static BOOL __stdcall
    stub_SetFilePointerEx(
        HANDLE hFile,
        LARGE_INTEGER liDistanceToMove,
        PLARGE_INTEGER lpNewFilePointer,
        DWORD dwMoveMethod)
    {
        SetLastError(0);
        LONG distance_to_move_lo = liDistanceToMove.LowPart;
        LONG distance_to_move_hi = liDistanceToMove.HighPart;
        auto const result = SetFilePointer(hFile, distance_to_move_lo, &distance_to_move_hi, dwMoveMethod);
        if (result == INVALID_SET_FILE_POINTER) {
            auto const err = GetLastError();
            if (err == 0) goto ok;
            return FALSE;
        }
        ok:
        lpNewFilePointer->HighPart = distance_to_move_hi;
        lpNewFilePointer->LowPart = result;
        return TRUE;
    }

    static BOOL __stdcall
    stub_RegisterWaitForSingleObject(
        PHANDLE phNewWaitObject,
        HANDLE hObject,
        WAITORTIMERCALLBACK Callback,
        PVOID Context,
        ULONG dwMilliseconds,
        ULONG dwFlags)
    {
        return FALSE;
    }

    static BOOL __stdcall
    stub_UnregisterWaitEx(
        HANDLE WaitHandle,
        HANDLE CompletionEvent)
    {
        return FALSE;
    }

    static DWORD __stdcall
    stub_GetLongPathNameW(
        LPCTSTR lpszShortPath,
        LPTSTR lpszLongPath,
        DWORD cchBuffer)
    {
        auto const sz = static_cast<DWORD>(lstrlenW(lpszShortPath));
        auto const szz = sz + 1;
        if (szz > cchBuffer) return szz;
        ::memmove(lpszLongPath, lpszShortPath, szz * 2);
        return sz;
    }

    static HWND __stdcall
    stub_GetConsoleWindow() {
        return nullptr;
    }
} }

namespace sqt { namespace winapi {
    #if 0
    static void Q_NEVER_INLINE
    winapi_resolve_user32() {
        winapi_resolve(L"System32\\user32.dll",
            zz_funcs_user32.ptrs, zz_stub_funcs_user32.ptrs,
            &symnames_user32[0][0], stride(symnames_user32), count(symnames_user32));
    }

    static void Q_NEVER_INLINE
    winapi_resolve_shell32() {
        winapi_resolve(L"System32\\shell32.dll",
            zz_funcs_shell32.ptrs, zz_stub_funcs_shell32.ptrs,
            &symnames_shell32[0][0], stride(symnames_shell32), count(symnames_shell32));
    }
    #endif

    static void Q_NEVER_INLINE
    winapi_resolve_kernel32() {
        winapi_resolve(L"System32\\kernel32.dll",
            zz_funcs_kernel32.ptrs, zz_stub_funcs_kernel32.ptrs,
            &symnames_kernel32[0][0], stride(symnames_kernel32), count(symnames_kernel32));
    }

    #if 0
    static void Q_NEVER_INLINE
    winapi_resolve_dwmapi() {
        winapi_resolve(L"System32\\dwmapi.dll",
            zz_funcs_dwmapi.ptrs, zz_stub_funcs_dwmapi.ptrs,
            &symnames_dwmapi[0][0], stride(symnames_dwmapi), count(symnames_dwmapi));
    }

    static void Q_NEVER_INLINE
    winapi_resolve_wtsapi32() {
        winapi_resolve(L"System32\\wtsapi32.dll",
            zz_funcs_wtsapi32.ptrs, zz_stub_funcs_wtsapi32.ptrs,
            &symnames_wtsapi32[0][0], stride(symnames_wtsapi32), count(symnames_wtsapi32));
    }

    static void Q_NEVER_INLINE
    winapi_resolve_gdi32() {
        winapi_resolve(L"System32\\gdi32.dll",
            zz_funcs_gdi32.ptrs, zz_stub_funcs_gdi32.ptrs,
            &symnames_gdi32[0][0], stride(symnames_gdi32), count(symnames_gdi32));
    }

    static void Q_NEVER_INLINE
    winapi_resolve_imm32() {
        winapi_resolve(L"System32\\imm32.dll",
            zz_funcs_imm32.ptrs, zz_stub_funcs_imm32.ptrs,
            &symnames_imm32[0][0], stride(symnames_imm32), count(symnames_imm32));
    }

    static void Q_NEVER_INLINE
    winapi_resolve_advapi32() {
        winapi_resolve(L"System32\\advapi32.dll",
            zz_funcs_advapi32.ptrs, zz_stub_funcs_advapi32.ptrs,
            &symnames_advapi32[0][0], stride(symnames_advapi32), count(symnames_advapi32));
    }
    #endif
} }
