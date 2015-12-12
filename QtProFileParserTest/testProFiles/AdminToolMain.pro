TEMPLATE = app

QT += qml quick widgets network xml quickwidgets
win32: QT += winextras

TARGET = HSCompanionAdminTool
VERSION = 1.0.0


DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES *= QT_USE_QSTRINGBUILDER

#include(../../QColorEdit/qcoloredit.pri)

include(./pluginmanager.pri)
include(./translations/translations.pri)

HEADERS += \
	mainwindow.h \
    settingsdialog.h \
    exchangemanager.h \
	startupmanager.h

SOURCES += main.cpp \
	mainwindow.cpp \
    settingsdialog.cpp \
    exchangemanager.cpp \
	startupmanager.cpp

FORMS += \
	mainwindow.ui \
	settingsdialog.ui

RESOURCES += \
    adminToolMain_res.qrc

CONFIG(debug, debug|release) {
	HEADERS += modeltest.h
	SOURCES += modeltest.cpp
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

win32 {
	RC_ICONS += ./resources/hs_koblenz.ico
	QMAKE_TARGET_COMPANY = "Hochschule Koblenz"
	QMAKE_TARGET_PRODUCT = "HS-Companion Admintool"
	QMAKE_TARGET_DESCRIPTION = $$QMAKE_TARGET_PRODUCT#"A tool to create timetables for the HS Koblenz"
	RC_LANG = 7

	DEFINES += "COMPANY=\"\\\"$$QMAKE_TARGET_COMPANY\\\"\""
	DEFINES += "DISPLAY_NAME=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\""

	LIBS += -lUser32
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../AdminToolCore/release/ -lAdminToolCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../AdminToolCore/debug/ -lAdminToolCored

baum42 | CONFIG(baum, baum|42): RC_LANG += 8

INCLUDEPATH += $$PWD/../AdminToolCore
DEPENDPATH += $$PWD/../AdminToolCore
