TRANSLATIONS += $$PWD/AdminToolMain_de.ts \
	$$PWD/AdminToolMain_en.ts

DISTFILES += $$PWD/AdminToolMain_de.ts \
	$$PWD/AdminToolMain_en.ts

lupdate_only{
	SOURCES += $$PWD/../*.qml \
		$$PWD/../*.js
}
