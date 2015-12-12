#ifndef PROJECTSCOPE_H
#define PROJECTSCOPE_H

#include <QCoreApplication>
#include <QString>
#include <QList>
#include <QScopedPointer>
#include "qprojectfieldmap.h"

#define regex(x) QRegularExpression(literal(x))
#define literal(x) literalWrap(regex_delimiter( ## x ## )regex_delimiter)
#define literalWrap(x) R#x

class ProjectScope
{
	Q_DECLARE_TR_FUNCTIONS(ProjectScope)

public:
	ProjectScope(const QString &rawString);
	~ProjectScope();

	bool isValid() const;
	QString evaluateConditions(const QStringList &conditions);

	QString extractFields(QProjectFieldMap &fieldMap);

private:
	enum EqualType {
		Set,
		Add,
		Remove,
		Replace,
		Invalid
	};

	QString scopeString;
	QList<ProjectScope*> subScopes;
	bool valid;

	bool verifyCondition(QString condString, bool acceptNextElse, const QStringList &conditions, bool &isElseCondition);
	QStringList splitWithConfig(QString &orConditions);

	static EqualType stringToType(const QString &string);
};

#endif // PROJECTSCOPE_H
