#include <QtCore/private/qglobal_p.h>

#ifdef Q_OS_WIN
    //#include "../src/corelib/kernel/qeventdispatcher_win_p.h"
    //#include "../src/corelib/kernel/qwineventnotifier.h"
    //#include "../src/corelib/io/qwindowspipewriter_p.h"
    //#include "../src/corelib/io/qwindowspipereader_p.h"

    #include "moc_qeventdispatcher_win_p.cpp"
    #if QT_CONFIG(filesystemwatcher)
        #include "moc_qfilesystemwatcher_win_p.cpp"
    #endif
    #include "moc_qwindowspipereader_p.cpp"
    #include "moc_qwindowspipewriter_p.cpp"
    #include "moc_qwineventnotifier.cpp"

#endif

#if QT_CONFIG(mimetype)
    #include "qrc_mimetypes.cpp"
#endif
