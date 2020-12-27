#pragma once
#include "qtcore-config.h"

// -----------------------------------------------------------------------------
// generally useful
// -----------------------------------------------------------------------------
#define QT_FEATURE_wheelevent 1
#define QT_FEATURE_tabletevent 1
#define QT_FEATURE_draganddrop 1
#define QT_FEATURE_harfbuzz 1

// needed by qwindows
#define QT_FEATURE_systemtrayicon 1
#define QT_FEATURE_clipboard 1
#define QT_FEATURE_sessionmanager 1
#define QT_FEATURE_dynamicgl -1

// -----------------------------------------------------------------------------
// questionable utility
// -----------------------------------------------------------------------------
#define QT_FEATURE_opengl 1
#define QT_FEATURE_opengles2 -1
#define QT_FEATURE_whatsthis 1
#define QT_FEATURE_shortcut 1
#define QT_FEATURE_standarditemmodel 1

// -----------------------------------------------------------------------------
// useless
// -----------------------------------------------------------------------------

#define QT_FEATURE_texture_format_astc_experimental -1

#define QT_FEATURE_cssparser -1
#if !defined(QT_NO_CSSPARSER)
    #define QT_NO_CSSPARSER
#endif

#if !defined(QT_NO_TEXTODFWRITER)
    #define QT_NO_TEXTODFWRITER
#endif

#if !defined(QT_NO_PDF)
    #define QT_NO_PDF
#endif

#if !defined(QT_NO_TEXTHTMLPARSER)
    #define QT_NO_TEXTHTMLPARSER
#endif

#if !defined(QT_NO_DEBUGSTREAM)
    #define QT_NO_DEBUGSTREAM
#endif

// #if !defined(QT_NO_VALIDATOR)
//     #define QT_NO_VALIDATOR
// #endif

// QT_NO_DATASTREAM breaks:
// qbmphandler.cpp, qiconloader.cpp, qicon.cpp, qpaintengine_pic.cpp,
// qpicture.cpp, qstandarditemmode.cpp, qregion.cpp
//
// #if !defined(QT_NO_DATASTREAM)
//     #define QT_NO_DATASTREAM
// #endif

// -----------------------------------------------------------------------------
// XXXih: figure out later
// -----------------------------------------------------------------------------
#define QT_FEATURE_vulkan -1

// XPM pixmap support is needed by qwindowscursor.cpp
#define QT_FEATURE_imageformat_xpm 1
// #define QT_FEATURE_imageformat_xpm -1
// #if !defined(QT_NO_IMAGEFORMAT_XPM)
//     #define QT_NO_IMAGEFORMAT_XPM
// #endif

#define QT_FEATURE_imageformat_png 1
#if QT_FEATURE_imageformat_png != 1 && !defined(QT_NO_IMAGEFORMAT_PNG)
    #define QT_NO_IMAGEFORMAT_PNG
#endif

#define QT_FEATURE_xcb_xlib 1 // A libX11 Display is needed for OpenGL support.
#define QT_FEATURE_xcb_sm -1
#define QT_FEATURE_xcb_native_painting -1
#define QT_FEATURE_fontconfig 1
#define QT_FEATURE_multiprocess 1
