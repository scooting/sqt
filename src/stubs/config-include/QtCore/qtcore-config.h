#pragma once
#define QT_FEATURE_framework -1
#define QT_FEATURE_rpath -1
#define QT_FEATURE_accessibility 1
#define QT_FEATURE_shared 1
#define QT_FEATURE_debug_and_release -1
#define QT_FEATURE_simulator_and_device -1
#define QT_FEATURE_build_all -1
#define QT_FEATURE_c__11 1
#define QT_FEATURE_c__14 -1
#define QT_FEATURE_c__1z -1
#define QT_FEATURE_concurrent -1
#define QT_FEATURE_force_asserts -1
#define QT_FEATURE_pkg_config -1
#define QT_FEATURE_separate_debug_info -1
#define QT_FEATURE_static -1

// #define QT_LARGEFILE_SUPPORT 64

#define QT_NO_MREMAP
#define QT_NO_CONCURRENT
#define QT_NO_DBUS
#define QT_NO_CUPS
#define QT_NO_EVDEV
#define QT_NO_EVENTFD
#define QT_NO_GLIB
#define QT_NO_ICONV
#define QT_NO_IMAGEFORMAT_JPEG
#define QT_NO_INOTIFY
#define QT_NO_MTDEV
#define QT_NO_NIS
#define QT_NO_OPENVG
#define QT_NO_STYLE_GTK
#define QT_NO_STYLE_WINDOWSCE
#define QT_NO_STYLE_WINDOWSMOBILE
#define QT_NO_TSLIB

#if (defined(__amd64__) || defined(_M_AMD64)) || (defined(_M_IX86) || defined(__i386__))
    // XXXih: FIXME: SSSE3-AVX2 ought to be enabled.
    #define QT_COMPILER_SUPPORTS_SSE2 1
    #define QT_COMPILER_SUPPORTS_SSSE3 1
    #define QT_COMPILER_SUPPORTS_SSE4_1 1
    #define QT_COMPILER_SUPPORTS_AVX2 1
#endif

#define QT_NO_FONTCONFIG

// XXXih: ???
// #define QT_NO_NATIVE_GESTURES


// XXXih:
//
#define QT_FEATURE_big_codecs 1
#define QT_FEATURE_codecs 1
#define QT_FEATURE_commandlineparser 1
#define QT_FEATURE_cxx11_future -1
#define QT_FEATURE_future 1
#define QT_FEATURE_datestring 1
#define QT_FEATURE_filesystemwatcher 1
#define QT_FEATURE_library 1
#define QT_FEATURE_regularexpression 1
#define QT_FEATURE_settings 1
#define QT_FEATURE_std_atomic64 1
#define QT_FEATURE_textcodec 1
#define QT_FEATURE_textdate 1
#define QT_FEATURE_thread 1
#define QT_FEATURE_timezone 1
#define QT_FEATURE_topleveldomain -1
#define QT_FEATURE_processenvironment 1
#define QT_FEATURE_process 1
#define QT_FEATURE_translation 1
#define QT_FEATURE_mimetype 1
#define QT_FEATURE_temporaryfile 1
#define QT_FEATURE_datetimeparser 1

#define QT_FEATURE_identityproxymodel 1
#define QT_FEATURE_itemmodel 1
#define QT_FEATURE_proxymodel 1
#define QT_FEATURE_sortfilterproxymodel 1
#define QT_FEATURE_stringlistmodel 1

#define QT_FEATURE_slog2 -1
#define QT_FEATURE_journald -1
#define QT_FEATURE_syslog -1

// -----------------------------------------------------------------------------
// useless
// -----------------------------------------------------------------------------

// #define QT_NO_DEBUG_STREAM

#define QT_FEATURE_gestures -1
#define QT_NO_GESTURES 1

#define QT_FEATURE_animation -1
#define QT_FEATURE_qeventtransition -1

// XXXih: This is needed to make platform plugin loading work.
// #define QT_NO_FILESYSTEMITERATOR 1

// XXXih: non-windows??
#define QT_FEATURE_getentropy -1

// XXXih: used by Qt 5.15.0
#define QT_FEATURE_signaling_nan 1
#define QT_FEATURE_easingcurve -1
#define QT_FEATURE_zstd -1
#define QT_FEATURE_jalalicalendar -1
#define QT_FEATURE_islamiccivilcalendar -1
#define QT_FEATURE_relocatable -1
#define QT_FEATURE_mimetype_database -1
#define QT_FEATURE_textmarkdownreader -1
#define QT_FEATURE_textmarkdownwriter -1
#define QT_FEATURE_texthtmlparser -1
#define QT_FEATURE_raster_64bit -1
#define QT_FEATURE_cursor 1

#if defined(_WIN32)
    #define QT_FEATURE_dlopen -1
#else
    #define QT_FEATURE_dlopen 1
#endif

// XXXih: required by qtranslator, qabstractitemmodel.cpp
// #define QT_NO_DATASTREAM 1

// #define QT_NO_TRANSLATION 1 // depends on QDataStream

// XXXih: uses datastream, transitive dependency of anything using QDataStream
#define QT_FEATURE_cborstreamreader 1
#define QT_FEATURE_cborstreamwriter 1

// XXXih: needed by qfactoryloader.cpp, which is needed by qlibrary.cpp
#define QT_FEATURE_binaryjson 1

// XXXih: needed for qregularexpression.cpp
#define QT_USE_QSTRINGBUILDER 1

#if defined(__linux__)
    #define QT_CONFIGURE_SETTINGS_PATH "/etc/xdg"
    #define QT_FEATURE_getauxval -1
    #define QT_FEATURE_linkat -1
    #define QT_FEATURE_statx -1
    #define QT_FEATURE_glibc -1
    #define QT_FEATURE_renameat2 -1

    #define QT_FEATURE_poll_pollts -1
    #define QT_FEATURE_poll_poll 1
    #define QT_FEATURE_poll_ppoll 1
    #define QT_FEATURE_poll_select 1

    #define QT_FEATURE_futimes 1
    #define QT_FEATURE_futimens 1
#endif
