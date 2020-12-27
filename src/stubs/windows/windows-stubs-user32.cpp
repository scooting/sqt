// #define SQT_WINSTUBS_NO_PREPROC_FORWARDING

#include <initguid.h>
// #include <shlguid.h>
// #include <wincrypt.h>

#include "windows-stubs.hpp"
#include "windows-stubs-util.hpp"

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_user32();
} }

#define SQT_WINSTUBS_IMPLEMENTATION_user32
#define SQT_WINSTUBS_NO_FPTR_DECL
#include "windows-stubs-gen.hpp"
#undef SQT_WINSTUBS_NO_FPTR_DECL

namespace sqt { namespace winapi {
    static BOOL __stdcall
    stub_RegisterTouchWindow(HWND, ULONG) {
        return FALSE;
    }

    static BOOL __stdcall
    stub_UnregisterTouchWindow(HWND) {
        return TRUE;
    }

    static BOOL __stdcall
    stub_IsTouchWindow(HWND, PULONG caps) {
        *caps = 0;
        return FALSE;
    }

    static BOOL __stdcall
    stub_CloseTouchInputHandle(HTOUCHINPUT) {
        return TRUE;
    }

    static BOOL __stdcall
    stub_GetTouchInputInfo(HTOUCHINPUT, UINT, PTOUCHINPUT, int) {
        return FALSE;
    }

    static BOOL __stdcall
    stub_UpdateLayeredWindowIndirect(HWND, const UPDATELAYEREDWINDOWINFO *) {
        return FALSE;
    }

    static BOOL __stdcall
    stub_ChangeWindowMessageFilterEx(
        HWND hwnd,
        UINT message,
        DWORD action,
        PCHANGEFILTERSTRUCT pChangeFilterStruct)
    {
        return TRUE;
    }

    static HDEVNOTIFY __stdcall
    stub_RegisterDeviceNotificationW(
        HANDLE hRecipient,
        LPVOID NotificationFilter,
        DWORD Flags)
    {
        return nullptr;
    }

    static BOOL __stdcall
    stub_UnregisterDeviceNotification(
        HDEVNOTIFY Handle)
    {
        return FALSE;
    }

    static BOOL __stdcall
    stub_SetLayeredWindowAttributes(
        HWND hwnd,
        COLORREF crKey,
        BYTE bAlpha,
        DWORD dwFlags)
    {
        return FALSE;
    }

    static BOOL __stdcall
    stub_UpdateLayeredWindow(
        HWND hwnd,
        HDC hdcDst,
        POINT *pptDst,
        SIZE *psize,
        HDC hdcSrc,
        POINT *pptSrc,
        COLORREF crKey,
        BLENDFUNCTION *pblend,
        DWORD dwFlags)
    {
        return FALSE;
    }

    static HMONITOR __stdcall
    stub_MonitorFromPoint(
        POINT pt,
        DWORD dwFlags)
    {
        return reinterpret_cast<HMONITOR>(0x1u);
    }

    static HMONITOR __stdcall
    stub_MonitorFromWindow(
        HWND hwnd,
        DWORD dwFlags)
    {
        return reinterpret_cast<HMONITOR>(0x1u);
    }

    static BOOL __stdcall
    stub_EnumDisplayMonitors(
        HDC hdc,
        LPCRECT lprcClip,
        MONITORENUMPROC lpfnEnum,
        LPARAM dwData)
    {
        OutputDebugStringA("stub_EnumDisplayMonitors\n");
        const auto w = GetSystemMetrics(SM_CXSCREEN);
        const auto h = GetSystemMetrics(SM_CYSCREEN);
        auto rect = RECT { 0, 0, w, h };
        lpfnEnum(reinterpret_cast<HMONITOR>(0x1u), nullptr, &rect, dwData);
        return TRUE;
    }

    static BOOL __stdcall
    stub_GetMonitorInfoW(
        HMONITOR hMonitor,
        LPMONITORINFO lpmi)
    {
        RECT work = { 0, 0, 0, 0 };
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
        const auto w = GetSystemMetrics(SM_CXSCREEN);
        const auto h = GetSystemMetrics(SM_CYSCREEN);
        lpmi->rcWork = work;
        lpmi->rcMonitor = { 0, 0, w, h };
        lpmi->dwFlags = MONITORINFOF_PRIMARY;
        if (lpmi->cbSize == sizeof(MONITORINFOEXW)) {
            auto const ex = reinterpret_cast<MONITORINFOEXW *>(lpmi);
            ::memset(ex->szDevice, 0, CCHDEVICENAME * sizeof(WCHAR));
            //::memcpy(ex->szDevice, L"Default", sizeof(L"Default"));
        }
        return TRUE;
    }

    HPOWERNOTIFY __stdcall
    stub_RegisterPowerSettingNotification(
        HANDLE hRecipient,
        LPCGUID PowerSettingGuid,
        DWORD Flags)
    {
        return nullptr;
    }

    static BOOL __stdcall
    stub_UnregisterPowerSettingNotification(HPOWERNOTIFY Handle) {
        return TRUE;
    }
} }

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_user32() {
        winapi_resolve(L"System32\\user32.dll",
            zz_funcs_user32.ptrs, zz_stub_funcs_user32.ptrs,
            &symnames_user32[0][0], stride(symnames_user32), count(symnames_user32));
    }
} }
