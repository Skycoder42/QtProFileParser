#include "tst_qtprofileparsertest.h"
#include <QtXml>

QtProFileParserTest::QtProFileParserTest()
{
}

void QtProFileParserTest::parsingTest_data()
{
	QTest::addColumn<QString>("proFile");
	QTest::addColumn<QStringList>("conditions");
	QTest::addColumn<bool>("includes");
	QTest::addColumn<QProjectFieldMap::FieldList>("defaults");
	QTest::addColumn<QProjectFieldMap::FieldList>("results");

    this->scanXml(QDir::current().absoluteFilePath("./QtProFileParserTest/TestVector.xml"));
}

void QtProFileParserTest::parsingTest()
{
	QFETCH(QString, proFile);
	QFETCH(QStringList, conditions);
	QFETCH(bool, includes);
	QFETCH(QProjectFieldMap::FieldList, defaults);
	QFETCH(QProjectFieldMap::FieldList, results);

	QtProFileParser parser;
	parser.setFilePath(proFile)
			.setConditions(conditions)
			.setFollowIncludes(includes)
			.setDefaultFields(defaults);
	QProjectFieldMap map = parser.parse();
	QVERIFY2(map.isValid(), parser.errorString().toLocal8Bit().data());
	QString res = this->compare(map.entries(), results);
	QVERIFY2(res.isEmpty(), res.toLocal8Bit().data());
}

void QtProFileParserTest::scanXml(const QString &path)
{
	QDir::setCurrent(QFileInfo(path).dir().absolutePath());

	QFile file(path);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	QDomDocument doc;
	if(doc.setContent(&file)) {
		QDomElement root = doc.documentElement();
		if(root.tagName() == "TestData"){
			QDomNodeList vectors = root.elementsByTagName("TestVector");
			for(int i = 0; i < vectors.size(); i++) {
				QDomElement vector = vectors.at(i).toElement();
				//add row
				QTestData &data = QTest::newRow(vector.attribute("name", QString::number(i)).toUtf8().data());

				//read filename
				QDomElement file = vector.elementsByTagName("File").at(0).toElement();
				data << file.attribute("path");

				//read conditions
				QDomElement conditions = vector.elementsByTagName("Conditions").at(0).toElement();
				QDomNodeList conds = conditions.elementsByTagName("Condition");
				QStringList cs;
				for(int j = 0; j < conds.size(); j++)
					cs << conds.at(j).toElement().attribute("value");
				cs.removeAll(QString());
				data << cs;

				//read include
				QDomElement includes = vector.elementsByTagName("Includes").at(0).toElement();
				data << (includes.attribute("follow") == "true");

				//read default state
				QDomElement defaults = vector.elementsByTagName("DefaultFields").at(0).toElement();
				if(defaults.attribute("custom") == "true")
					data << this->extractFieldMap(defaults);
				else
					data << QtProFileParser::standardDefaultFields;

				//read result
				QDomElement results = vector.elementsByTagName("Result").at(0).toElement();
				data << this->extractFieldMap(results);
			}
		}
	}

	file.close();
}

QProjectFieldMap::FieldList QtProFileParserTest::extractFieldMap(QDomElement &element)
{
	QProjectFieldMap::FieldList fieldList;
	QDomNodeList fields = element.elementsByTagName("Field");
	for(int i = 0; i < fields.size(); i++) {
		QDomElement field = fields.at(i).toElement();

		//get the entries
		QStringList ents;
		QDomNodeList entries = field.elementsByTagName("Entry");
		for(int j = 0; j < entries.size(); j++)
			ents << entries.at(j).toElement().attribute("value");
		ents.removeAll(QString());

		//get name and add to map
		QString name = field.attribute("name");
		if(!name.isEmpty())
			fieldList.append({name, ents});
	}

	return fieldList;
}

QString QtProFileParserTest::compare(const QProjectFieldMap::FieldList &left, const QProjectFieldMap::FieldList &right)
{
	QStringList globalKeys;
	QMap<QString, QProjectFieldMap::FieldEntry> leftMap;
	foreach(QProjectFieldMap::FieldEntry entry, left){
		leftMap.insert(entry.key, entry);
		globalKeys << entry.key;
	}

	QMap<QString, QProjectFieldMap::FieldEntry> rightMap;
	foreach(QProjectFieldMap::FieldEntry entry, right){
		rightMap.insert(entry.key, entry);
		globalKeys << entry.key;
	}

	globalKeys.removeDuplicates();
	foreach(QString key, globalKeys) {
		if(!(leftMap.value(key) == rightMap.value(key)))
			return QString("Error:\n"
						   "Map:        %2\n"
						   "TestVector: %3\n"
						   "Different Values for key \"%1\"")
					.arg(key)
					.arg(leftMap.value(key).values.join(" "))
					.arg(rightMap.value(key).values.join(" "));
	}

	return QString();
}

QTEST_GUILESS_MAIN(QtProFileParserTest)
