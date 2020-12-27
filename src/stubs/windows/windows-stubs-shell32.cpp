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

#include <initguid.h>
#include <shlguid.h>
// #include <wincrypt.h>

#include "windows-stubs.hpp"
#include "windows-stubs-util.hpp"

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_shell32();
} }

#define SQT_WINSTUBS_IMPLEMENTATION_shell32
#define SQT_WINSTUBS_NO_FPTR_DECL
#include "windows-stubs-gen.hpp"
#undef SQT_WINSTUBS_NO_FPTR_DECL

namespace sqt { namespace winapi {
    struct fid_to_csidl_t {
        GUID fid;
        int csidl;
    };

    static fid_to_csidl_t const fid_to_csidl[] = {
        { FOLDERID_Desktop, CSIDL_DESKTOPDIRECTORY },
        { FOLDERID_Documents, CSIDL_MYDOCUMENTS },
        { FOLDERID_Fonts , CSIDL_FONTS },
        { FOLDERID_Programs, CSIDL_PROGRAMS },
        { FOLDERID_Music, CSIDL_MYMUSIC },
        { FOLDERID_Videos, CSIDL_MYVIDEO },
        { FOLDERID_Pictures, CSIDL_MYPICTURES },
        { FOLDERID_LocalAppData, CSIDL_LOCAL_APPDATA },
        { FOLDERID_RoamingAppData, CSIDL_APPDATA },
        { FOLDERID_Downloads, CSIDL_MYDOCUMENTS },
        { FOLDERID_ProgramData, CSIDL_COMMON_APPDATA },
    };

    static HRESULT WINAPI
    stub_SHGetKnownFolderPath(
        REFKNOWNFOLDERID rfid,
        DWORD dwFlags,
        HANDLE hToken,
        PWSTR *ppszPath)
    {
        wchar_t buf[MAX_PATH + 1];
        buf[MAX_PATH] = 0;
        int folder_id = -1;

        for (auto const &assoc : fid_to_csidl) {
            if (assoc.fid == rfid) {
                folder_id = assoc.csidl;
                break;
            }
        }
        if (folder_id == -1) {
            return E_NOTIMPL;
        }
        auto const success = SHGetSpecialFolderPathW(nullptr, buf, folder_id, FALSE);
        if (!success) {
            return E_NOTIMPL;
        }
        auto const len = lstrlenW(buf);
        auto const ptr = static_cast<wchar_t *>(CoTaskMemAlloc(sizeof(wchar_t) * (len + 1)));
        memcpy(ptr, buf, sizeof(wchar_t) * len);
        ptr[len] = 0;
        *ppszPath = ptr;
        return S_OK;
    }

    static ::HRESULT __stdcall
    stub_SHGetKnownFolderIDList(
        _In_ REFKNOWNFOLDERID rfid,
        _In_ DWORD /* KNOWN_FOLDER_FLAG */ dwFlags,
        _In_opt_ HANDLE hToken,
        _Outptr_ PIDLIST_ABSOLUTE *ppidl)
    {
        return E_NOTIMPL;
    }

    static ::HRESULT __stdcall
    stub_SHCreateItemFromParsingName(
        _In_ PCWSTR pszPath,
        _In_opt_ IBindCtx *pbc,
        _In_ REFIID riid,
        _Outptr_ void **ppv)
    {
        return E_NOTIMPL;
    }

    static ::HRESULT __stdcall
    stub_SHCreateItemFromIDList(
        _In_ PCIDLIST_ABSOLUTE pidl,
        _In_ REFIID riid,
        _Outptr_ void **ppv)
    {
        return E_NOTIMPL;
    }

    static ::HRESULT __stdcall
    stub_SHGetStockIconInfo(
        SHSTOCKICONID siid,
        UINT uFlags,
        _Inout_ SHSTOCKICONINFO *psii)
    {
        return E_NOTIMPL;
    }

    static ::HRESULT __stdcall
    stub_Shell_NotifyIconGetRect(
        _In_ const NOTIFYICONIDENTIFIER* identifier,
        _Out_ RECT* iconLocation)
    {
        return E_NOTIMPL;
    }

    static ::HRESULT __stdcall
    stub_SHGetImageList(int, REFIID, void **) {
        return E_NOTIMPL;
    }

    static BOOL __stdcall
    stub_SHGetSpecialFolderPathW(
        HWND hwndOwner,
        LPTSTR lpszPath,
        int nFolder,
        BOOL fCreate)
    {
        return FALSE;
    }
} }

namespace sqt { namespace winapi {
    static void Q_NEVER_INLINE
    winapi_resolve_shell32() {
        winapi_resolve(L"System32\\shell32.dll",
            zz_funcs_shell32.ptrs, zz_stub_funcs_shell32.ptrs,
            &symnames_shell32[0][0], stride(symnames_shell32), count(symnames_shell32));
    }
} }
