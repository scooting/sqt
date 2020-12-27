#include <QtNetwork/private/qtnetwork-config_p.h>

#include "moc_qabstractnetworkcache.cpp"
#include "moc_qabstractsocketengine_p.cpp"
#include "moc_qhostinfo_p.cpp"
#include "moc_qnativesocketengine_p.cpp"
#include "moc_qnetworkaccessbackend_p.cpp"
#include "moc_qnetworkaccesscache_p.cpp"
#include "moc_qnetworkaccessfilebackend_p.cpp"
#include "moc_qnetworkcookiejar.cpp"
#include "moc_qnetworkfile_p.cpp"
#include "moc_qnetworkproxy.cpp"
#include "moc_qnetworkreply.cpp"
#include "moc_qtcpsocket.cpp"
#include "moc_qudpsocket.cpp"

#if QT_CONFIG(http)
    #include "moc_qhttp2protocolhandler_p.cpp"
    #include "moc_qhttpmultipart.cpp"
    #include "moc_qhttpnetworkreply_p.cpp"
    #include "moc_qhttpsocketengine_p.cpp"
    #include "moc_qhttpthreaddelegate_p.cpp"
    #include "moc_qnetworkreplyhttpimpl_p.cpp"
#endif
