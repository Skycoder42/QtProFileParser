#ifndef TST_QTPROFILEPARSERTEST_H
#define TST_QTPROFILEPARSERTEST_H

#include <QString>
#include <QtTest>
#include <QDomElement>
#include "qtprofileparser.h"

class QtProFileParserTest : public QObject
{
	Q_OBJECT

public:
	QtProFileParserTest();

private Q_SLOTS:
	void parsingTest_data();
	void parsingTest();

private:
	void scanXml(const QString &path);
	QProjectFieldMap::FieldList extractFieldMap(QDomElement &element);

	QString compare(QProjectFieldMap::FieldList &left, QProjectFieldMap::FieldList &right);
};

#endif // TST_QTPROFILEPARSERTEST_H

