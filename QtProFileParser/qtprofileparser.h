#ifndef QTPROFILEPARSER_H
#define QTPROFILEPARSER_H

#include <QObject>
#include <QDir>
#include "qprojectfieldmap.h"

//! \internal internal class
class ProjectScope;

//! A simple parser to scan .pro files and extract it's fields
class QtProFileParser : public QObject
{
	Q_OBJECT

	//! The path to the project file to scan
	Q_PROPERTY(QString filePath MEMBER proFilePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
	//! "Conditions" to be set when evaluation the projects conditions
	Q_PROPERTY(QStringList conditions MEMBER proConditions READ conditions WRITE setConditions NOTIFY conditionsChanged)
	//! Specifies whether to follow include(...) or not
	Q_PROPERTY(bool followIncludes MEMBER useIncludes READ followIncludes WRITE setFollowIncludes NOTIFY followIncludesChanged)
	//! A list of default fields, that are set before parsing
	Q_PROPERTY(QProjectFieldMap::FieldList defaultFields MEMBER defaultMap READ defaultFields WRITE setDefaultFields NOTIFY defaultFieldsChanged)
public:
	//! A basic collection of standard default fields qmake uses
	static const QProjectFieldMap::FieldList standardDefaultFields;

	//! Constructs a new parser
	QtProFileParser(QObject *parent = NULL);

	//! Returns true, if parsing failed
	bool hasError() const;
	//! Returns a string to descibe an parse-error
	QString errorString() const;

	//! READ-ACCESSOR for QtProFileParser::filePath
	QString filePath() const;
	//! READ-ACCESSOR for QtProFileParser::conditions
	QStringList conditions() const;
	//! READ-ACCESSOR for QtProFileParser::followIncludes
	bool followIncludes() const;
	//! READ-ACCESSOR for QtProFileParser::defaultFields
	QProjectFieldMap::FieldList defaultFields() const;

	//! Sets an input-device to be used to get the project file.
	QtProFileParser &setInput(QIODevice *device, bool clearPath = true);

	//! Adds a condition to the currently set ones
	QtProFileParser &addCondition(const QString &condition);
	//! Adds a field to the currently set fields.
	QtProFileParser &addField(QProjectFieldMap::FieldEntry entry);

public slots:
	//! WRITE-ACCESSOR for QtProFileParser::filePath
	QtProFileParser &setFilePath(QString filePath, bool clearDevice = true);
	//! WRITE-ACCESSOR for QtProFileParser::conditions
	QtProFileParser &setConditions(QStringList conditions);
	//! WRITE-ACCESSOR for QtProFileParser::followIncludes
	QtProFileParser &setFollowIncludes(bool followIncludes);
	//! WRITE-ACCESSOR for QtProFileParser::defaultFields
	QtProFileParser &setDefaultFields(QProjectFieldMap::FieldList defaultFields);

	//! Parses the project file with all set parameters
	QProjectFieldMap parse();

signals:
	//! Emitted as soon as the parser finishes successfully
	void parseFinished(QProjectFieldMap fieldMap);
	//! Emitted if the parser finishes with an error
	void parseError(QString error);

	//! NOTIFY-ACCESSOR for QtProFileParser::filePath
	void filePathChanged(QString filePath);
	//! NOTIFY-ACCESSOR for QtProFileParser::conditions
	void conditionsChanged(QStringList conditions);
	//! NOTIFY-ACCESSOR for QtProFileParser::followIncludes
	void followIncludesChanged(bool followIncludes);
	//! NOTIFY-ACCESSOR for QtProFileParser::defaultFields
	void defaultFieldsChanged(QProjectFieldMap::FieldList defaultFields);

private:
	QIODevice *device;
	QString proFilePath;
	QStringList proConditions;
	bool useIncludes;
	QProjectFieldMap::FieldList defaultMap;
	QString error;

	//! \internal performs the actual parsing
	QProjectFieldMap parseFunction(QIODevice *device, bool viaFile);
	//! \internal loads include(...) statements
	bool loadIncludes(QString &data, QDir parent);
	//! \internal removes comments from the file
	QString removeComments(const QString &proArray);
	//! \internal remerges the file, removing \\ with the real thing
	QString remergeFile(const QString &original);

	//! \internal creates a scopetree out of the profile
	ProjectScope *createScopeTree(QString &projectData);
	//! \internal cleans conditions by removing whitespaces
	QStringList cleanConditions(const QStringList &proConditions);
};

#endif // QTPROFILEPARSER_H
