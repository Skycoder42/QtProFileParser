#-------------------------------------------------
#
# Project created by QtCreator 2015-07-09T21:40:08
#
#-------------------------------------------------

#IMPORTANT: to make the the test run successfully, you will need to set the working directory to %{sourceDir}

QT       += testlib xml
QT       -= gui

TARGET = tst_qtprofileparsertest
CONFIG   += console C++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_qtprofileparsertest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QtProFileParser/release/ -lQtProFileParser
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QtProFileParser/debug/ -lQtProFileParserd
else:mac:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QtProFileParser/ -lQtProFileParser
else:mac:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QtProFileParser/ -lQtProFileParser_debug
else:unix: LIBS += -L$$OUT_PWD/../QtProFileParser/ -lQtProFileParser

INCLUDEPATH += $$PWD/../QtProFileParser
DEPENDPATH += $$PWD/../QtProFileParser

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/release/libQtProFileParser.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/debug/libQtProFileParserd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/release/QtProFileParser.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/debug/QtProFileParserd.lib
else:mac:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/libQtProFileParser.a
else:mac:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/libQtProFileParser_debug.a
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../QtProFileParser/libQtProFileParser.a

DISTFILES += \
	TestVector.xml

HEADERS += \
    tst_qtprofileparsertest.h

SUBDIRS += \
    testProFiles/QtProFileParser.pro \
    testProFiles/QtProFileParserTest.pro
