#-------------------------------------------------
#
# Project created by QtCreator 2015-07-09T21:40:08
#
#-------------------------------------------------

QT       += testlib xml

QT       -= gui

TARGET = tst_qtprofileparsertest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_qtprofileparsertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QtProFileParser/release/ -lQtProFileParser
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QtProFileParser/debug/ -lQtProFileParser
else:unix: LIBS += -L$$OUT_PWD/../QtProFileParser/ -lQtProFileParser

INCLUDEPATH += $$PWD/../QtProFileParser
DEPENDPATH += $$PWD/../QtProFileParser

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/release/libQtProFileParser.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/debug/libQtProFileParser.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/release/QtProFileParser.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/debug/QtProFileParser.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/libQtProFileParser.a

DISTFILES += \
    TestVector.xml

HEADERS += \
    tst_qtprofileparsertest.h
