#include "projectscope.h"
#include <QCoreApplication>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringList>

ProjectScope::ProjectScope(const QString &rawString) :
	scopeString(rawString),
	subScopes(),
	valid(false)
{
	int scopeCnt = 0;
	int scopeDepth = 0;
	int scopeBegin = 0;
	for(int i = 0; i < this->scopeString.size(); i++) {
		const QChar &ch = this->scopeString[i];

		if(ch == '{'){
			if(scopeDepth == 0)
				scopeBegin = i;
			scopeDepth++;
		} else if(ch == '}') {
			scopeDepth--;
			if(scopeDepth < 0)
				return;
			else if(scopeDepth == 0) {
				//check if scope or variable
				if(scopeBegin > 2 && (this->scopeString.midRef(scopeBegin - 2, 3) == "$${")){
					scopeBegin = 0;
					continue;
				}
				//if one scope was read, extract and replace it
				QString subScopeString = this->scopeString.mid(scopeBegin + 1, i - scopeBegin - 1);
				this->scopeString.replace(scopeBegin, i - scopeBegin + 1, QString("#%1#").arg(scopeCnt++));
				i = scopeBegin;
				scopeBegin = 0;

				//create a subscope from it and check if valid
				this->subScopes.append(new ProjectScope(subScopeString));
				if(!this->subScopes.last()->valid)
					return;
			}
		}
	}

	this->valid = (scopeDepth == 0);
}

ProjectScope::~ProjectScope()
{
	qDeleteAll(this->subScopes);
}

bool ProjectScope::isValid() const
{
	return this->valid;
}

QString ProjectScope::evaluateConditions(const QStringList &conditions)
{
	QStringList lines = this->scopeString.split(QRegularExpression("([\\n\\r])+"));

	//reminders
	QList<int> remerges; //a list of lines to be merged with the lines before them
	bool acceptNextElse = false;//if true, the next else will be accepted
	bool isElseCondition = false;//if true the next condition can be else

	for(int i = 0; i < lines.size(); i++) {
		QString &line = lines[i];
		if(line.contains(QRegularExpression("#\\d+#"))) {
			//get begin and end of condition to evaluate
			QRegularExpressionMatch match;
			int condBegin = line.indexOf(QRegularExpression("([^:]+)(:([^:]+))*\\s*#\\d+#"));
			int condEnd = line.indexOf(QRegularExpression("#(\\d+)#"), condBegin, &match);
			if(condBegin == -1)
				return tr("subscope without condition detected");

			//get subscope number
			int id = match.captured(1).toInt();

			//verify the condition
			if(this->verifyCondition(line.mid(condBegin, condEnd - condBegin), acceptNextElse, conditions, isElseCondition)) {
				//following else will not be accepted
				acceptNextElse = false;
				//remove condition, but keep scope
				line.remove(condBegin, condEnd - condBegin);
				//eval valid subscopes conditions
				this->subScopes[id]->evaluateConditions(conditions);

				//re-append line, but splitted, to eval remaining conditions
				int splitIndex = line.indexOf(QRegularExpression("#\\d+#"));
				splitIndex = line.indexOf("#", splitIndex + 1) + 1;
				lines.insert(i + 1, line.mid(splitIndex));
				line.resize(splitIndex);
				remerges.append(i + 1);
			} else {
				//accept next else, if current wasn't else
				if(!isElseCondition)
					acceptNextElse = true;
				//delete the scope
				delete this->subScopes[id];
				this->subScopes[id] = NULL;
				//remove condition and scope
				int splitIndex = line.indexOf("#", condEnd + 1) + 1;
				line.remove(0, splitIndex);
				//decerement i to repeat the (cleaned) line
				i--;
			}

			//set next else, always after any condition
			isElseCondition = true;
		} else if(line.contains(":")) {
			//get begin and end of the "normal" condition
			int condBegin = line.indexOf(QRegularExpression("(([^:]+):)+"));
			int condEnd = line.lastIndexOf(":") + 1;
			if(this->verifyCondition(line.mid(condBegin, condEnd - condBegin), acceptNextElse, conditions, isElseCondition)) {
				//following else will not be accepted
				acceptNextElse = false;
				//remove condition (nothing can come before it), but keep the rest
				line.remove(0, condEnd);
			} else {
				//accept next else, if current wasn't else
				if(!isElseCondition)
					acceptNextElse = true;
				//remove the whole line and decrement i
				lines.removeAt(i);
				i--;
			}

			//set next else, always after any condition
			isElseCondition = true;
		} else {
			//no condition in this line -> next one isn't allowed to be else
			isElseCondition = false;
		}
	}

	//perform reminded remerges
	while(!remerges.isEmpty()) {
		lines[remerges.last() - 1].append(lines[remerges.last()]);
		lines.removeAt(remerges.last());
		remerges.removeLast();
	}
	//remove empty lines and join list
	this->scopeString = lines.filter(QRegularExpression("^.*\\S.*$")).join("\n");
	return QString();
}

QString ProjectScope::extractFields(QProjectFieldMap &fieldMap)
{
	QStringList lines = this->scopeString.split(QRegularExpression("([\\n\\r])+"));

	foreach(QString line, lines) {
		//simplify
		line = line.simplified();

		//check if is subScope, and if yes extract subscope
		QRegularExpressionMatch subScopeMatch = QRegularExpression(".*#(\\d+)#.*").match(line);
		if(subScopeMatch.hasMatch()) {
			QString err = this->subScopes[subScopeMatch.captured(1).toInt()]->extractFields(fieldMap);
			if(!err.isEmpty())
				return err;
			else
				continue;
		}

		//check if has =, +=, *=, -= or ~=
		QRegularExpressionMatch equalMatch = QRegularExpression("[+\\-\\*~]?=").match(line);
		if(equalMatch.hasMatch()) {
			EqualType type = ProjectScope::stringToType(equalMatch.captured(0));

			//extract the key
			QString key = line.mid(0, equalMatch.capturedStart(0)).simplified();
			if(key.contains(QRegularExpression("\\s")))
				return tr("Invalid key found. Key must be a single word");

			//extract the values
			QRegularExpressionMatchIterator splitMatch = QRegularExpression(R"__(((?<!\\)"(?:[^"]|(?:\\"))+(?<!\\)"|(?:\\"|[^\s"])+))__")
														 .globalMatch(line.mid(equalMatch.capturedEnd(0)));
			QStringList values;
			while(splitMatch.hasNext()) {
				QString match = splitMatch.next().captured(0);
				if(match.startsWith(QLatin1Char('"')) && match.endsWith(QLatin1Char('"')))
					match = match.mid(1, match.size() - 2);
				values += match;
			}

			//change the fieldMap, depending on the type
			QStringList oldVals = fieldMap.value(key);
			switch(type) {
			case Set:
				fieldMap.insert(key, values);
				break;
			case Add:
				oldVals.append(values);
				oldVals.removeDuplicates();
				fieldMap.insert(key, oldVals);
				break;
			case Remove:
				foreach (QString subVal, values)
					oldVals.removeAll(subVal);
				fieldMap.insert(key, oldVals);
				break;
			case Replace:
				//extract the expression adn replacement and check if valid
				values = line.mid(equalMatch.capturedEnd(0))
						 .split("/", QString::SkipEmptyParts);
				if(values.size() > 3 || values.size() < 2)
					return tr("Invalid replace value found");
				if(values.size() == 3)
					values.removeFirst();

				//replace all
				oldVals.replaceInStrings(QRegularExpression(values[0]), values[1]);
				fieldMap.insert(key, oldVals);
				break;
			default://Invalid is ignored, because it will never happen, due to programm logic
				break;
			}

			//continue because the rest cant be valid now
			continue;
		}
	}

	return QString();
}

bool ProjectScope::verifyCondition(QString condString, bool acceptNextElse, const QStringList &conditions, bool &isElseCondition)
{
	//prepare
	condString.remove(QRegularExpression("\\s"));
	QStringList conds = condString.split(":", QString::SkipEmptyParts);

	//check if this condition is else, and if yes if accepted
	if(conds.first() == "else") {
		if(!isElseCondition)
			return false;//TODO exception
		if(acceptNextElse)
			conds.removeFirst();
		else
			return false;
	} else
		isElseCondition = false;

	//go through all required conditions
	foreach (QString condition, conds) {
		bool hasCondition = false;
		//go through all subconditions, where only one must be true
		QStringList condField = this->splitWithConfig(condition);
		foreach(QString orCond, condField) {
			//check if the condition is negated
			bool negate = orCond.startsWith("!");
			orCond.remove(QRegularExpression::escape("!"));

			if(negate) {//if negated and not in conditions -> success
				if(!conditions.contains(orCond)) {
					hasCondition = true;
					break;
				}
			} else {//if not negated and in conditions -> success
				if(conditions.contains(orCond)) {
					hasCondition = true;
					break;
				}
			}
		}

		//check if one of the or-conditions succeeded, if not return false
		if(!hasCondition)
			return false;
	}

	//all conditions succeeded
	return true;
}

QStringList ProjectScope::splitWithConfig(QString &orConditions)
{
	//remebers
	QStringList configList;
	//regex
	QRegularExpression regexp("CONFIG\\(.*\\)");
	QRegularExpressionMatch configMatch = regexp.match(orConditions);
	if(configMatch.hasMatch()) {
		//remove and save all the configs
		do {
			QString configStr = configMatch.captured(0);
			orConditions.replace(configMatch.capturedStart(0),
								 configMatch.capturedLength(0),
								 QString("#%1#").arg(configList.size()));
			configList.append(configStr);
			//rematch because the string changed
			configMatch = regexp.match(orConditions);
		} while(configMatch.hasMatch());

		//split the list and reinsert the removed config-functions
		QStringList result = orConditions.split("|", QString::SkipEmptyParts);
		for(int i = 0; i < result.size(); i++) {
			QRegularExpressionMatch idMatch = QRegularExpression("#(\\d+)#").match(result[i]);
			if(idMatch.hasMatch()) {
				result[i].replace(idMatch.capturedStart(0),
								  idMatch.capturedLength(0),
								  configList[idMatch.captured(1).toInt()]);
			}
		}

		//return the new list
		return result;
	} else {
		return orConditions.split("|", QString::SkipEmptyParts);
	}
}

ProjectScope::EqualType ProjectScope::stringToType(const QString &string)
{
	if(string == "=")
		return Set;
	else if(string == "+=" || string == "*=")
		return Add;//TODO difference between add and *
	else if(string == "-=")
		return Remove;
	else if(string == "~=")
		return Replace;
	else
		return Invalid;
}
