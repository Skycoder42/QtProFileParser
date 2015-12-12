#include "qprojectfieldmap.h"
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <QDir>

QProjectFieldMap::QProjectFieldMap() :
	QMap(),
	valid(false),
	path(),
	conds()
{}

QProjectFieldMap::QProjectFieldMap(const QProjectFieldMap &other) :
	QMap(other),
	valid(other.valid),
	path(other.path),
	conds(other.conds)
{}

bool QProjectFieldMap::isValid() const
{
	return this->valid;
}

QString QProjectFieldMap::filePath() const
{
	return this->path;
}

QStringList QProjectFieldMap::conditions() const
{
	return this->conds;
}

QProjectFieldMap::FieldEntry QProjectFieldMap::entry(const QString &key) const
{
	return {key, this->value(key)};
}

QProjectFieldMap::FieldList QProjectFieldMap::entries() const
{
	FieldList list;
	QStringList keys = this->keys();
	foreach (QString key, keys)
		list.append({key, this->value(key)});
	return list;
}

QJsonObject QProjectFieldMap::createJsonObject() const
{
	QJsonObject root;
	if(this->path.isEmpty())
		root["proFile"] = QJsonValue(QJsonValue::Null);
	else
		root["proFile"] = QDir::current().absoluteFilePath(this->path);
	root["conditions"] = QJsonArray::fromStringList(this->conds);

	QJsonObject fields;
	QStringList keys = this->keys();
	foreach(QString key, keys)
		fields[key] = QJsonArray::fromStringList(this->value(key));

	root["fields"] = fields;
	return root;
}

QByteArray QProjectFieldMap::createJsonString(bool indented) const
{
	return QJsonDocument(this->createJsonObject()).toJson(indented ? QJsonDocument::Indented : QJsonDocument::Compact);
}



bool operator==(const QProjectFieldMap::FieldEntry &left, const QProjectFieldMap::FieldEntry &right)
{
	if(left.key == right.key) {
		QStringList lVals = left.values;
		QStringList rVals = right.values;
		lVals.sort();
		rVals.sort();
		return lVals == rVals;
	} else
		return false;
}

bool operator!=(const QProjectFieldMap::FieldEntry &left, const QProjectFieldMap::FieldEntry &right)
{
	if(left.key != right.key)
		return true;
	else {
		QStringList lVals = left.values;
		QStringList rVals = right.values;
		lVals.sort();
		rVals.sort();
		return lVals != rVals;
	}
}

bool operator==(const QProjectFieldMap::FieldList &left, const QProjectFieldMap::FieldList &right)
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
			return false;
	}

	return true;
}

bool operator!=(const QProjectFieldMap::FieldList &left, const QProjectFieldMap::FieldList &right)
{
	QStringList globalKeys;
	QMap<QString, QProjectFieldMap::FieldEntry> leftMap;
	foreach(QProjectFieldMap::FieldEntry entry, left){
		leftMap.insert(entry.key, entry);
		globalKeys << entry.key;
	}

	QMap<QString, QProjectFieldMap::FieldEntry> rightMap;
	foreach(QProjectFieldMap::FieldEntry entry, right){
		leftMap.insert(entry.key, entry);
		globalKeys << entry.key;
	}

	globalKeys.removeDuplicates();
	foreach(QString key, globalKeys) {
		if(!(leftMap.value(key) != rightMap.value(key)))
			return false;
	}

	return true;
}
