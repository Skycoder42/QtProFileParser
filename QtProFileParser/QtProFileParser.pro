#-------------------------------------------------
#
# Project created by QtCreator 2015-07-09T21:38:37
#
#-------------------------------------------------

QT       -= gui
CONFIG += C++11

TARGET = $$qtLibraryTarget(QtProFileParser)
TEMPLATE = lib
CONFIG += staticlib

SOURCES += qtprofileparser.cpp \
    projectscope.cpp \
    qprojectfieldmap.cpp

HEADERS += qtprofileparser.h \
	qprojectfieldmap.h \
	projectscope.h
