SOURCES += tst_pairdiagnostics.cpp
QT = core testlib

CONFIG -= app_bundle debug_and_release_target

TARGET = pairdiagnostics

include($$QT_SOURCE_TREE/src/testlib/selfcover.pri)
