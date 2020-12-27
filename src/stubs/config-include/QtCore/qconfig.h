#pragma once
#define QT_VERSION_MAJOR    5
#define QT_VERSION_MINOR    15
#define QT_VERSION_PATCH    2
#define QT_VERSION_STR      "5.15.2"

#define QT_PRODUCT_LICENSEE "Open Source"
#define QT_PRODUCT_LICENSE "OpenSource"

#if defined(_WIN32)
    #define QT_QPA_DEFAULT_PLATFORM_NAME "windows"
#endif

#if defined(__linux__)
    #define QT_VISIBILITY_AVAILABLE 1
    #define QT_QPA_DEFAULT_PLATFORM_NAME "xcb"
#endif

#include "qtcore-config.h"
