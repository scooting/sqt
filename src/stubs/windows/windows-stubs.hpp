#include <QtCore/qglobal.h>

#if defined(Q_OS_WINDOWS)

#include <utility>

#if !defined(WIN32_LEAN_AND_MEAN)
    #define WIN32_LEAN_AND_MEAN
#endif

#if !defined(NOMINMAX)
    #define NOMINMAX
#endif

#if !defined(STRICT_TYPED_ITEMIDS)
    #define STRICT_TYPED_ITEMIDS
#endif

#include <windows.h>
#include <shlobj.h>

#include <shellapi.h> // SHSTOCKICONID
#include <dwmapi.h> // DWM_BLURBEHIND
#include <wtsapi32.h> // WTS_INFO_CLASS

#define SQT_WINSTUBS_NO_IMPLEMENTATION
#include "windows-stubs-gen.hpp"
#undef SQT_WINSTUBS_NO_IMPLEMENTATION

#endif //defined(Q_OS_WINDOWS)