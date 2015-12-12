#include "qtprofileparser.h"
#include <QFile>
#include <QFileInfo>
#include <QScopedPointer>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QFutureWatcher>
#include "projectscope.h"

static QProjectFieldMap::FieldList createDefaultMap();
const QProjectFieldMap::FieldList QtProFileParser::standardDefaultFields(createDefaultMap());

QtProFileParser::QtProFileParser(QObject *parent) :
	QObject(parent),
	device(NULL),
	proFilePath(),
	proConditions(),
	useIncludes(true),
	defaultMap(QtProFileParser::standardDefaultFields),
	error()
{}

bool QtProFileParser::hasError() const
{
	return !(this->error.isEmpty());
}

QString QtProFileParser::errorString() const
{
	return this->error;
}

QString QtProFileParser::filePath() const
{
	return this->proFilePath;
}

QStringList QtProFileParser::conditions() const
{
	return this->proConditions;
}

bool QtProFileParser::followIncludes() const
{
	return this->useIncludes;
}

QtProFileParser &QtProFileParser::setInput(QIODevice *device, bool clearPath)
{
	this->device = device;
	if(clearPath && !this->proFilePath.isEmpty()){
		this->proFilePath.clear();
		emit filePathChanged(QString());
	}
	return (*this);
}

QtProFileParser &QtProFileParser::addCondition(const QString &condition)
{
	if(!this->proConditions.contains(condition)) {
		this->proConditions.append(condition);
		emit conditionsChanged(this->proConditions);
	}
	return (*this);
}

QtProFileParser &QtProFileParser::addField(QProjectFieldMap::FieldEntry entry)
{
	if(!this->defaultMap.contains(entry)) {
		this->defaultMap.append(entry);
		emit defaultFieldsChanged(this->defaultMap);
	}
	return (*this);
}

QProjectFieldMap::FieldList QtProFileParser::defaultFields() const
{
	return this->defaultMap;
}

QtProFileParser &QtProFileParser::setFilePath(QString filePath, bool clearDevice)
{
	if(clearDevice)
		this->device = NULL;
	if (this->proFilePath == filePath)
		return (*this);

	this->proFilePath = filePath;
	emit filePathChanged(filePath);
	return (*this);
}

QtProFileParser &QtProFileParser::setConditions(QStringList conditions)
{
	if (this->proConditions == conditions)
		return (*this);

	this->proConditions = conditions;
	emit conditionsChanged(conditions);
	return (*this);
}

QtProFileParser &QtProFileParser::setFollowIncludes(bool followIncludes)
{
	if (this->useIncludes == followIncludes)
		return (*this);

	this->useIncludes = followIncludes;
	emit followIncludesChanged(followIncludes);
	return (*this);
}

QtProFileParser &QtProFileParser::setDefaultFields(QProjectFieldMap::FieldList defaultFields)
{
	if (this->defaultMap == defaultFields)
		return (*this);

	this->defaultMap = defaultFields;
	emit defaultFieldsChanged(defaultFields);
	return (*this);
}

QProjectFieldMap QtProFileParser::parse()
{
	QProjectFieldMap resultMap;
	if(this->device == NULL) {
		QFile projectFile(this->proFilePath);
		if(projectFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			resultMap = this->parseFunction(&projectFile, true);
			projectFile.close();
			if(resultMap.valid)
				resultMap.path = this->proFilePath;
		} else//file open failed
			this->error = tr("Unable to open file %1").arg(this->proFilePath);
	} else
		resultMap = this->parseFunction(this->device, false);

	//check if valid to emit and return
	if(resultMap.valid) {
		emit parseFinished(resultMap);
		return resultMap;
	} else {
		emit parseError(this->error);
		return QProjectFieldMap();
	}
}

QProjectFieldMap QtProFileParser::parseFunction(QIODevice *device, bool viaFile)
{
	//cleanup and setup
	this->error.clear();
	QProjectFieldMap valueMap;
	valueMap.conds = this->proConditions;
	foreach(QProjectFieldMap::FieldEntry entry, this->defaultMap)
		valueMap.insert(entry.key, entry.values);

	//read file
	QString proData = QString::fromUtf8(device->readAll());
	if(viaFile && this->useIncludes) {
		if(!this->loadIncludes(proData, QFileInfo(this->proFilePath).dir())) {
			qWarning("Unable to load at least one project include");
			return QProjectFieldMap();
		}
	} else
		proData = this->removeComments(proData);
	proData = this->remergeFile(proData);

	//scope data and evalute conditions
	QScopedPointer<ProjectScope> scope(this->createScopeTree(proData));
	if(!scope->isValid()) {
		this->error = tr("Invalid file. The number of opening and closing brackets {} is unequal!");
		return QProjectFieldMap();
	}
	this->error = scope->evaluateConditions(this->cleanConditions(this->proConditions));
	if(!this->error.isEmpty())
		return QProjectFieldMap();

	//perform the actual parse to get all fields
	this->error = scope->extractFields(valueMap);
	if(!this->error.isEmpty())
		return QProjectFieldMap();

	//finish with validation
	valueMap.valid = true;
	return valueMap;
}

bool QtProFileParser::loadIncludes(QString &data, QDir parent)
{//TODO prevent crash from recursive includes
	int start = 0;
	int end = 0;
	forever {//search for include(...) statements
		data = this->removeComments(data);
		start = data.indexOf("include(", start);
		if(start == -1)
			break;
		end = data.indexOf(")", start);

		//load the file inside the statement
		QString name = data.mid(start + 8, end - (start + 8));
		QFile priFile(parent.absoluteFilePath(name));
		QString content;
		if(priFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			content = QString::fromUtf8(priFile.readAll());
			priFile.close();
		} else
			return false;

		//replace the statement with the files contents
		if(end == -1)
			data.replace(start, -1, content);
		else
			data.replace(start, end - start + 1, content);
	}

	return true;
}

QString QtProFileParser::removeComments(const QString &proArray)
{
	QStringList lines = proArray.split(QRegularExpression("([\\n\\r])+"));
	for(int i = 0; i < lines.size(); i++) {
		lines[i].remove(QRegularExpression("#.*$"));
		if(lines[i].isEmpty())
			lines.removeAt(i--);
	}
	return lines.join("\n");
}

QString QtProFileParser::remergeFile(const QString &original)
{
	QStringList lines = original.split(QRegularExpression("([\\n\\r])+"));
	QString remerged;
	foreach(QString line, lines) {
		if(line.endsWith("\\"))
			line.replace(line.size() - 1, 1, " ");
		else
			line.append("\n");//seperator
		remerged.append(line);
	}
	return remerged;
}

ProjectScope *QtProFileParser::createScopeTree(QString &projectData)
{
	ProjectScope *scope = new ProjectScope(projectData);
	return scope;
}

QStringList QtProFileParser::cleanConditions(const QStringList &conditions)
{
	QStringList result;
	foreach(QString condition, conditions)
		result.append(condition.remove(QRegularExpression("\\s")));
	return result;
}



static QProjectFieldMap::FieldList createDefaultMap()
{
	QProjectFieldMap::FieldList map;
	map.append({
				   "QT",
				   QStringList()
						<< "core"
						<< "gui"
			   });
	map.append({
				   "CONFIG",
				   QStringList()
					   << "app_bundle"
					   << "debug_and_release_target"
					   << "exceptions"
					   << "qt"
					   << "thread"
					   << "flat"
					   << "embed_manifest_exe"
					   << "embed_manifest_dll"
			   });
	return map;
}

static void initParserLib()
{
	qRegisterMetaType<QProjectFieldMap>("QProjectFieldMap");
	qRegisterMetaType<QProjectFieldMap::FieldEntry>("QProjectFieldMap::FieldEntry");
	qRegisterMetaType<QProjectFieldMap::FieldList>("QProjectFieldMap::FieldList");
}
Q_COREAPP_STARTUP_FUNCTION(initParserLib)
