#pragma once

#define QT_FEATURE_http 1


// Temporarily disabled while I figure out OpenSSL.
#define QT_FEATURE_schannel -1
#define QT_FEATURE_ssl -1
#define QT_FEATURE_openssl -1
#define QT_NO_OPENSSL 1
#define QT_NO_SSL 1

// -----------------------------------------------------------------------------
// useless
// -----------------------------------------------------------------------------

#define QT_FEATURE_ftp -1
#define QT_FEATURE_dtls -1

#define QT_FEATURE_sctp -1
#define QT_NO_SCTP 1

#define QT_FEATURE_sspi -1

#define QT_FEATURE_gssapi -1

#define QT_FEATURE_bearermanagement -1
#define QT_NO_BEARERMANAGEMENT 1

#define QT_FEATURE_networkproxy -1
#define QT_FEATURE_socks5 -1
#define QT_NO_NETWORKPROXY 1

// Needed by qnetworkproxy_win.cpp and qnativesocketengine_win.cpp.
#define QT_FEATURE_networkinterface 1
// #define QT_NO_NETWORKINTERFACE 1

// -----------------------------------------------------------------------------
// useless but enabled for now
// -----------------------------------------------------------------------------
