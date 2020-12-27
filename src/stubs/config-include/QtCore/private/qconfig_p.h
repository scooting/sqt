#define QT_FEATURE_private_tests -1
#define QT_FEATURE_dbus -1
#define QT_FEATURE_dbus_linked -1
#define QT_FEATURE_gui 1
#define QT_FEATURE_libudev -1
#define QT_FEATURE_posix_fallocate -1
#define QT_FEATURE_reduce_exports -1
#define QT_FEATURE_reduce_relocations -1
#define QT_FEATURE_release_tools 1
#define QT_FEATURE_system_zlib -1
#define QT_FEATURE_widgets 1

#if defined(__amd64__) || defined(_M_AMD64)
    #define QT_FEATURE_sse2 1
#else
    #define QT_FEATURE_sse2 -1
#endif

