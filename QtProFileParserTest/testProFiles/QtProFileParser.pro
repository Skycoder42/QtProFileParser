#-------------------------------------------------
#
# Project created by QtCreator 2015-07-09T21:38:37
#
#-------------------------------------------------

QT       -= gui

TARGET = QtProFileParser
TEMPLATE = lib
CONFIG += staticlib

SOURCES += qtprofileparser.cpp \
    projectscope.cpp \
    qprojectfieldmap.cpp

HEADERS += \
	projectscope.h

unix {
    target.path = /usr/lib
    INSTALLS += target
} else:win32 {
	DEFINES += BAUM
} else {
	DEFINES += BAUM42
}

baum42: VERSION = 1.0 1.1 "1.2 1.3" 1.4

include(public.pri)
