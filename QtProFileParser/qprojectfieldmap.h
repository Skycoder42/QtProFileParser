#ifndef PROJECTFIELDMAP_H
#define PROJECTFIELDMAP_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QJsonObject>

//! A map containing all fields of a parsed Qt Project
class QProjectFieldMap : public QMap<QString, QStringList>
{
	Q_GADGET

	//! The path of this file. Can be empty
	Q_PROPERTY(QString filePath READ filePath)
	//! The conditions that have been active while parsing the file
	Q_PROPERTY(QStringList conditions READ conditions)

	//! \internal allow write access for the parser
	friend class QtProFileParser;
public:
	//! A structure representing one field-entry in the project file
	struct FieldEntry {
		//! The key of the field
		QString key;
		//! The values assigned to the key
		QStringList values;
	};
	//! A list of fieldEntries, can be used to compare project files
	class FieldList : public QList<FieldEntry>{};

	//! Creates a new, empty and invalid map
	QProjectFieldMap();
	//! Creates a new map as a copy of other
	QProjectFieldMap(const QProjectFieldMap &other);

	//! Returns true, if the map contains valid data
	bool isValid() const;
	//! READ-Accessor for QProjectFieldMap::filePath
	QString filePath() const;
	//! READ-Accessor for QProjectFieldMap::conditions
	QStringList conditions() const;

	//! Returns the entry for the key
	Q_INVOKABLE FieldEntry entry(const QString &key) const;
	//! Returns all entries  of this map as a list
	Q_INVOKABLE FieldList entries() const;

	//! Converts the map into a JSON-Object
	QJsonObject createJsonObject() const;
	//! Converts the map into a JSON-Object and returns it as a string
	Q_INVOKABLE QByteArray createJsonString(bool indented = true) const;

private:
	bool valid;
	QString path;
	QStringList conds;
};

bool operator==(const QProjectFieldMap::FieldEntry &left, const QProjectFieldMap::FieldEntry &right);
bool operator!=(const QProjectFieldMap::FieldEntry &left, const QProjectFieldMap::FieldEntry &right);
bool operator==(const QProjectFieldMap::FieldList &left, const QProjectFieldMap::FieldList &right);
bool operator!=(const QProjectFieldMap::FieldList &left, const QProjectFieldMap::FieldList &right);

Q_DECLARE_METATYPE(QProjectFieldMap)
Q_DECLARE_METATYPE(QProjectFieldMap::FieldEntry)
Q_DECLARE_METATYPE(QProjectFieldMap::FieldList)

#endif // PROJECTFIELDMAP_H
