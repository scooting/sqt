let core_funcs = [
  (* Introduced in Windows XP. *)
  ("advapi32", "SystemFunction036");
  ("kernel32", "GetNativeSystemInfo");
  (* Introduced in Windows XP SP1/Server 2003. *)
  ("kernel32", "GetProcessId");
  (* Introduced in Windows Vista. *)
  ("shell32", "SHGetKnownFolderPath");
  ("kernel32", "CancelIoEx");
  (* Introduced in Windows 2000. *)
  ("kernel32", "GetVolumeNameForVolumeMountPointW");
  ("kernel32", "GetVolumePathNameW");
  ("kernel32", "SetFilePointerEx");
  ("kernel32", "RegisterWaitForSingleObject");
  ("kernel32", "UnregisterWaitEx");
  ("kernel32", "GetLongPathNameW");
  ("kernel32", "GetConsoleWindow");
  ("user32", "RegisterDeviceNotificationW");
  ("user32", "UnregisterDeviceNotification");
  ("shell32", "SHGetSpecialFolderPathW");
]

let qwindows_funcs = [
  (* Introduced in Windows 7. *)
  ("user32", "RegisterTouchWindow");
  ("user32", "UnregisterTouchWindow");
  ("user32", "IsTouchWindow");
  ("user32", "CloseTouchInputHandle");
  ("user32", "GetTouchInputInfo");
  (* Introduced in Windows Vista. *)
  ("shell32", "SHGetKnownFolderIDList");
  ("shell32", "SHCreateItemFromParsingName");
  ("shell32", "SHCreateItemFromIDList");
  ("shell32", "SHGetStockIconInfo");
  ("shell32", "Shell_NotifyIconGetRect");
  ("user32", "UpdateLayeredWindowIndirect");
  ("user32", "ChangeWindowMessageFilterEx");
  ("user32", "RegisterPowerSettingNotification");
  ("user32", "UnregisterPowerSettingNotification");
  ("dwmapi", "DwmIsCompositionEnabled");
  ("dwmapi", "DwmEnableBlurBehindWindow");
  ("dwmapi", "DwmSetWindowAttribute");
  ("dwmapi", "DwmGetWindowAttribute");
  (* Introduced in Windows XP. *)
  ("shell32", "SHGetImageList");
  ("kernel32", "CheckRemoteDebuggerPresent");
  ("kernel32", "WTSGetActiveConsoleSessionId");
  (* Introduced in Windows 2000. *)
  ("wtsapi32", "WTSQuerySessionInformationW");
  ("wtsapi32", "WTSFreeMemory");
  ("gdi32", "AddFontResourceExW");
  ("gdi32", "AddFontMemResourceEx");
  ("gdi32", "RemoveFontResourceExW");
  ("gdi32", "RemoveFontMemResourceEx");
  ("gdi32", "SetLayout");
  ("imm32", "ImmAssociateContextEx");
  ("user32", "SetLayeredWindowAttributes");
  ("user32", "UpdateLayeredWindow");
  ("user32", "MonitorFromPoint");
  ("user32", "MonitorFromWindow");
  ("user32", "EnumDisplayMonitors");
  ("user32", "GetMonitorInfoW");
]

type ctype = Ctype of string | Const of ctype | Ptr of ctype

let blendfunction = Ctype "BLENDFUNCTION"
let bool' = Ctype "BOOL"
let boolean = Ctype "BOOLEAN"
let byte = Ctype "BYTE"
let colorref = Ctype "COLORREF"
let dwm_blurbehind = Ctype "DWM_BLURBEHIND"
let dword = Ctype "DWORD"
let handle = Ctype "HANDLE"
let hdc = Ctype "HDC"
let hdevnotify = Ctype "HDEVNOTIFY"
let himc = Ctype "HIMC"
let hmonitor = Ctype "HMONITOR"
let hpowernotify = Ctype "HPOWERNOTIFY"
let hresult = Ctype "HRESULT"
let htouchinput = Ctype "HTOUCHINPUT"
let hwnd = Ctype "HWND"
let ibindctx_ptr = Ctype "IBindCtx *"
let int' = Ctype "int"
let large_integer = Ctype "LARGE_INTEGER"
let lparam = Ctype "LPARAM"
let lpcguid = Ctype "LPCGUID"
let lpcrect = Ctype "LPCRECT"
let lpcvoid = Ctype "LPCVOID"
let lpcwstr = Ctype "LPCWSTR"
let lpmonitorinfo = Ctype "LPMONITORINFO"
let lpoverlapped = Ctype "LPOVERLAPPED"
let lpsystem_info = Ctype "LPSYSTEM_INFO"
let lpvoid = Ctype "LPVOID"
let lpwstr = Ctype "LPWSTR"
let monitorenumproc = Ctype "MONITORENUMPROC"
let notifyiconidentifier = Ctype "NOTIFYICONIDENTIFIER"
let pbool = Ctype "PBOOL"
let pchangefilterstruct = Ctype "PCHANGEFILTERSTRUCT"
let pcidlist_absolute = Ctype "PCIDLIST_ABSOLUTE"
let pcwstr = Ctype "PCWSTR"
let phandle = Ctype "PHANDLE"
let pidlist_absolute_ptr = Ctype "PIDLIST_ABSOLUTE *"
let plarge_integer = Ctype "PLARGE_INTEGER"
let point = Ctype "POINT"
let ptouchinput = Ctype "PTOUCHINPUT"
let pulong = Ctype "PULONG"
let pvoid = Ctype "PVOID"
let pwstr = Ctype "PWSTR"
let rect = Ctype "RECT"
let refiid = Ctype "REFIID"
let refknownfolderid = Ctype "REFKNOWNFOLDERID"
let shstockiconid = Ctype "SHSTOCKICONID"
let shstockiconinfo_ptr = Ctype "SHSTOCKICONINFO *"
let size = Ctype "SIZE"
let uint = Ctype "UINT"
let ulong = Ctype "ULONG"
let updatelayeredwindowinfo = Ctype "UPDATELAYEREDWINDOWINFO"
let void = Ctype "void"
let waitortimercallback = Ctype "WAITORTIMERCALLBACK"
let wts_info_class = Ctype "WTS_INFO_CLASS"

let funcs_win7 = [
  ("user32", "RegisterTouchWindow",   bool', [("hwnd", hwnd); ("ulFlags", ulong)]);
  ("user32", "UnregisterTouchWindow", bool', [("hwnd", hwnd)]);
  ("user32", "IsTouchWindow",         bool', [("hwnd", hwnd); ("pulFlags", pulong)]);
  ("user32", "CloseTouchInputHandle", bool', [("hTouchInput", htouchinput)]);
  ("user32", "GetTouchInputInfo",     bool', [("hTouchInput", htouchinput); ("cInputs", uint); ("pInputs", ptouchinput); ("cbSize", int')]);
]

let funcs_vista = [
  ("shell32",  "SHGetKnownFolderPath",               hresult,      [("rfid", refknownfolderid); ("dwFlags", dword); ("hToken", handle); ("ppszPath", Ptr pwstr)]);
  ("kernel32", "CancelIoEx",                         bool',        [("hFile", handle); ("lpOverlapped", lpoverlapped)]);
  ("shell32",  "SHGetKnownFolderIDList",             hresult,      [("rfid", refknownfolderid); ("dwFlags", dword); ("hToken", handle); ("ppidl", pidlist_absolute_ptr)]);
  ("shell32",  "SHCreateItemFromParsingName",        hresult,      [("pszPath", pcwstr); ("pbc", ibindctx_ptr); ("riid", refiid); ("ppv", Ptr (Ptr void))]);
  ("shell32",  "SHCreateItemFromIDList",             hresult,      [("pidl", pcidlist_absolute); ("riid", refiid); ("ppv", Ptr (Ptr void))]);
  ("shell32",  "SHGetStockIconInfo",                 hresult,      [("siid", shstockiconid); ("uFlags", uint); ("psii", shstockiconinfo_ptr)]);
  ("shell32",  "Shell_NotifyIconGetRect",            hresult,      [("identifier", Ptr (Const notifyiconidentifier)); ("iconLocation", Ptr rect)]);
  ("user32",   "UpdateLayeredWindowIndirect",        bool',        [("hwnd", hwnd); ("pULWInfo", Ptr (Const updatelayeredwindowinfo))]);
  ("user32",   "ChangeWindowMessageFilterEx",        bool',        [("hwnd", hwnd); ("message", uint); ("action", dword); ("pChangeFilterStruct", pchangefilterstruct)]);
  ("user32",   "RegisterPowerSettingNotification",   hpowernotify, [("hRecipient", handle); ("PowerSettingGuid", lpcguid); ("Flags", dword)]);
  ("user32",   "UnregisterPowerSettingNotification", bool',        [("Handle", hpowernotify)]);
  ("dwmapi",   "DwmIsCompositionEnabled",            hresult,      [("pfEnabled", Ptr bool')]);
  ("dwmapi",   "DwmEnableBlurBehindWindow",          hresult,      [("hWnd", hwnd); ("pBlurBehind", Ptr (Const dwm_blurbehind))]);
  ("dwmapi",   "DwmSetWindowAttribute",              hresult,      [("hwnd", hwnd); ("dwAttribute", dword); ("pvAttribute", lpcvoid); ("cbAttribute", dword)]);
  ("dwmapi",   "DwmGetWindowAttribute",              hresult,      [("hwnd", hwnd); ("dwAttribute", dword); ("pvAttribute", pvoid); ("cbAttribute", dword)]);
]

let funcs_xpsp1 = [
  ("kernel32", "GetProcessId", dword, [("Process", handle)]);
]

let funcs_xp = [
  ("advapi32", "SystemFunction036",            boolean, [("RandomBuffer", pvoid); ("RandomBufferLength", ulong)]);
  ("kernel32", "GetNativeSystemInfo",          void,    [("lpSystemInfo", lpsystem_info)]);
  ("shell32",  "SHGetImageList",               hresult, [("iImageList", int'); ("riid", refiid); ("ppvobj", Ptr (Ptr void))]);
  ("kernel32", "CheckRemoteDebuggerPresent",   bool',   [("hProcess", handle); ("pbDebuggerPresent", pbool)]);
  ("kernel32", "WTSGetActiveConsoleSessionId", dword,   []);
]

let funcs_2k = [
  ("kernel32", "GetVolumeNameForVolumeMountPointW", bool',      [("lpszVolumeMountPoint", lpcwstr); ("lpszVolumeName", lpwstr); ("cchBufferLength", dword)]);
  ("kernel32", "GetVolumePathNameW",                bool',      [("lpszFileName", lpcwstr); ("lpszVolumePathName", lpwstr); ("cchBufferLength", dword)]);
  ("kernel32", "SetFilePointerEx",                  bool',      [("hFile", handle); ("liDistanceToMove", large_integer); ("lpNewFilePointer", plarge_integer); ("dwMoveMethod", dword)]);
  ("kernel32", "RegisterWaitForSingleObject",       bool',      [("phNewWaitObject", phandle); ("hObject", handle); ("Callback", waitortimercallback); ("Context", pvoid); ("dwMilliseconds", ulong); ("dwFlags", ulong)]);
  ("kernel32", "UnregisterWaitEx",                  bool',      [("WaitHandle", handle); ("CompletionEvent", handle)]);
  ("kernel32", "GetLongPathNameW",                  dword,      [("lpszShortPath", lpcwstr); ("lpszLongPath", lpwstr); ("cchBuffer", dword)]);
  ("kernel32", "GetConsoleWindow",                  hwnd,       []);
  ("user32",   "RegisterDeviceNotificationW",       hdevnotify, [("hRecipient", handle); ("NotificationFilter", lpvoid); ("Flags", dword)]);
  ("user32",   "UnregisterDeviceNotification",      bool',      [("Handle", hdevnotify)]);
  ("shell32",  "SHGetSpecialFolderPathW",           bool',      [("hwnd", hwnd); ("pszPath", lpwstr); ("csidl", int'); ("fCreate", bool')]);
  ("wtsapi32", "WTSQuerySessionInformationW",       bool',      [("hServer", handle); ("SessionId", dword); ("WTSInfoClass", wts_info_class); ("ppBuffer", Ptr lpwstr); ("pBytesReturned", Ptr dword)]);
  ("wtsapi32", "WTSFreeMemory",                     void,       [("pMemory", pvoid)]);
  ("gdi32",    "AddFontResourceExW",                int',       [("name", lpcwstr); ("fl", dword); ("res", pvoid)]);
  ("gdi32",    "AddFontMemResourceEx",              handle,     [("pFileView", pvoid); ("cjSize", dword); ("pvReserved", pvoid); ("pNumFonts", Ptr dword)]);
  ("gdi32",    "RemoveFontResourceExW",             bool',      [("name", lpcwstr); ("fl", dword); ("pdv", pvoid)]);
  ("gdi32",    "RemoveFontMemResourceEx",           bool',      [("h", handle)]);
  ("gdi32",    "SetLayout",                         dword,      [("hdc", hdc); ("l", dword)]);
  ("imm32",    "ImmAssociateContextEx",             bool',      [("hWnd", hwnd); ("hIMC", himc); ("dwFlags", dword)]);
  ("user32",   "SetLayeredWindowAttributes",        bool',      [("hwnd", hwnd); ("crKey", colorref); ("bAlpha", byte); ("dwFlags", dword)]);
  ("user32",   "UpdateLayeredWindow",               bool',      [("hWnd", hwnd); ("hdcDst", hdc); ("pptDst", Ptr point); ("psize", Ptr size); ("hdcSrc", hdc); ("pptSrc", Ptr point); ("crKey", colorref); ("pblend", Ptr blendfunction); ("dwFlags", dword)]);
  ("user32",   "MonitorFromPoint",                  hmonitor,   [("pt", point); ("dwFlags", dword)]);
  ("user32",   "MonitorFromWindow",                 hmonitor,   [("hwnd", hwnd); ("dwFlags", dword)]);
  ("user32",   "EnumDisplayMonitors",               bool',      [("hdc", hdc); ("lprcClip", lpcrect); ("lpfnEnum", monitorenumproc); ("dwData", lparam)]);
  ("user32",   "GetMonitorInfoW",                   bool',      [("hMonitor", hmonitor); ("lpmi", lpmonitorinfo)]);
]

let merged = List.concat [funcs_win7; funcs_vista; funcs_xpsp1; funcs_xp; funcs_2k]

let grouped =
  let sort_by_dll (l, _, _, _) (r, _, _, _) = compare l r in
  let sorted = List.stable_sort sort_by_dll merged in
  let f (lastdll, current, acc) ((dll, _, _, _) as elem) =
    let (current, acc) = if dll = lastdll
      then (elem :: current, acc)
      else ([elem], (lastdll, List.rev current) :: acc)
    in
    (dll, current, acc)
  in
  match sorted with
  | [] -> []
  | (dll, _, _, _) :: _ ->
    let (dll, current, acc) = List.fold_left f (dll, [], []) sorted in
    let acc = (dll, List.rev current) :: acc in
    let acc = List.rev acc in
    List.map (fun (dll, syms) -> (dll, List.length syms, syms)) acc

let for_list xs iter = List.iter iter xs
let for_listi xs iter = List.iteri iter xs

let rec print_type out t = match t with
| Ctype s -> Printf.fprintf out "%s" s
| Ptr nested -> print_type out nested; Printf.fprintf out " *"
| Const nested -> print_type out nested; Printf.fprintf out " const"

let rec print_args out sep args =
  let first = ref true in
  for_list args (fun (name, type') ->
    (if not !first then Printf.fprintf out "%s" sep);
    print_type out type';
    Printf.fprintf out " %s" name;
    first := false)

let rec print_names out sep args =
  let first = ref true in
  for_list args (fun (name, type') ->
    (if not !first then Printf.fprintf out "%s" sep);
    Printf.fprintf out "%s" name;
    first := false)

let main () =
  let open Printf in
  printf "// Generated by gen-winfuncs.ml. Do not edit.\n\n";
  printf "namespace sqt { namespace winapi {\n";
  (
    printf "#if !defined(SQT_WINSTUBS_NO_FPTR_DECL)\n";
    for_list grouped (fun (dll, num_syms, syms) ->
      printf "struct zz_func_table_%s {\n" dll;
      printf "    union {\n";

      printf "        struct {\n";
      for_list syms (fun (_, name, ret_type, args) ->
        printf "            ";
        print_type stdout ret_type;
        printf " (__stdcall *_%s) (" name;
        print_args stdout ", " args;
        printf ");\n";
      );
      printf "        } funcs;\n";
      printf "        void *ptrs[%d];\n" num_syms;
      printf "    };\n";
      printf "};\n";
    );
    printf "#endif // !defined(SQT_WINSTUBS_NO_FPTR_DECL)\n\n"
  );
  for_list grouped (fun (dll, _, _) ->
    printf "extern zz_func_table_%s zz_funcs_%s;\n\n" dll dll;
  );
  (
    printf "#if !defined(SQT_WINSTUBS_NO_IMPLEMENTATION)\n";
    (
      for_list grouped (fun (dll, num_syms, syms) ->
        let print_table_init pfx =
          for_list syms (fun (_, name, _, _) ->
            printf "    &%s%s,\n" pfx name
          )
        in
        printf "#if defined(SQT_WINSTUBS_IMPLEMENTATION_%s)\n" dll;
        for_list syms (fun (_, name, ret_type, args) ->
          printf "static ";
          print_type stdout ret_type;
          printf " __stdcall\n";

          printf "resolve_%s(" name;
          print_args stdout ", " args;
          printf ")\n";
          printf "{\n";
          printf "    ::sqt::winapi::winapi_resolve_%s();\n" dll;
          printf "    return ::sqt::winapi::zz_funcs_%s.funcs._%s(" dll name;
          print_names stdout ", " args;
          printf ");\n";

          printf "}\n\n"
        );
        for_list syms (fun (_, name, ret_type, args) ->
          printf "static ";
          print_type stdout ret_type;
          printf " __stdcall\n";
          printf "stub_%s(" name;
          print_args stdout ", " args;
          printf ");\n\n";
        );
        printf "static zz_func_table_%s const zz_stub_funcs_%s = {\n" dll dll;
        print_table_init "stub_";
        printf "};\n\n";

        printf "zz_func_table_%s zz_funcs_%s = {\n" dll dll;
        print_table_init "resolve_";
        printf "};\n";

        let symname_max_size =
          let f max_acc (_, name, _, _) = max (1 + String.length name) max_acc in
          List.fold_left f 1 syms
        in
        printf "static char symnames_%s[%d][%d] = {\n" dll num_syms symname_max_size;
        for_listi syms (fun i (_, name, _, _) ->
          printf "    /* %d */ \"%s\",\n" i name
        );
        printf "};\n";

        printf "#endif\n\n"
      )
    );
    printf "#endif // !defined(SQT_WINSTUBS_NO_IMPLEMENTATION)\n\n"
  );
  printf "} } // namespace sqt::winapi\n\n";

  (
    printf "#if !defined(SQT_WINSTUBS_NO_PREPROC_FORWARDING)\n";
    for_list grouped (fun (dll, _, syms) ->
      for_list syms (fun (_, name, _, _) ->
        printf "    #if defined(%s)\n" name;
        printf "        #undef %s\n" name;
        printf "    #endif\n";
        printf "    #define %s sqt::winapi::zz_funcs_%s.funcs._%s\n\n" name dll name
      );
    );
    printf "#endif // !defined(SQT_WINSTUBS_NO_PREPROC_FORWARDING)\n\n"
  );
  ()

;;
main ()
