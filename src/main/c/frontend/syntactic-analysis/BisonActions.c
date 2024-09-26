#include "BisonActions.h"
#include "BisonParser.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeBisonActionsModule() {
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

QuantifierType convertToQuantifier(char * quantifier) {
	if (strcmp(quantifier, ">=")){
		return GREATEREQUALS;
	} else if (strcmp(quantifier, ">")){
		return GREATERTHAN;
	} else if (strcmp(quantifier, "<=")){
		return LESSEREQUALS;
	} else if (strcmp(quantifier, "<")){
		return LESSERTHAN;
	} else {
		return EQUALS;
	}
}

/* PUBLIC FUNCTIONS */
Program * ProgramSemanticAction(CompilerState * compilerState, Query * query) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->query = query;
	compilerState->abstractSyntaxtTree = program;
	if (0 < flexCurrentContext()) {
		logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
		compilerState->succeed = false;
	}
	else {
		compilerState->succeed = true;
	}
	return program;
}


Query * ExpressionQuerySemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Query * query = calloc(1, sizeof(Query));
	query->mainQuery = expression;
	return query;
}

Query * ExpressionWithOrderProgramSemanticAction(Expression * expression, Metaorder * order) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Query * query = calloc(1, sizeof(Query));
	query->mainQuery = expression;
	query->order = order;
	return query;
}

Query * ExpressionSubquerySemanticAction(Subqueries * subqueries, Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Query * query = calloc(1, sizeof(Query));
	query->mainQuery = expression;
	query->subqueries = subqueries;
	return query;
}

Query * ExpressionSubqueryOrderedSemanticAction(Subqueries * subqueries, Expression * expression, Metaorder * order) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Query * query = calloc(1, sizeof(Query));
	query->mainQuery = expression;
	query->subqueries = subqueries;
	query->order = order;
	return query;
}


Subqueries * SubquerySingleSemanticAction(Subquery * subquery) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Subqueries * subqueries = calloc(1, sizeof(Subqueries));
	subqueries->subquery = subquery;
	return subqueries;
}

Subqueries * SubqueryRecursiveSemanticAction(Subquery * subquery, Subqueries * remaining) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Subqueries * subqueries = calloc(1, sizeof(Subqueries));
	subqueries->subquery = subquery;
	subqueries->subqueries = remaining;
	return subqueries;
}


Subquery * SubquerySemanticAction(Subqueryname * name, Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Subquery * subquery = calloc(1, sizeof(Subquery));
	subquery->name = name;
	subquery->expression = expression;
	return subquery;
}


Subqueryname * SubquerynameSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Subqueryname * subqueryname = calloc(1, sizeof(Subqueryname));
	subqueryname->name = name;
	return subqueryname;
}


Metaorder * MetaorderSemanticAction(Ordertypenode * order, boolean descending) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Metaorder * metaorder = calloc(1, sizeof(Metaorder));
	metaorder->order = order;
	metaorder->desc = descending;
	return metaorder;
}


Ordertypenode * OrdertypeSemanticAction(Token order) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Ordertypenode * ordertype = calloc(1, sizeof(Ordertypenode));
	switch(order){
		case ORDERCREATIONDATE:
			ordertype->order = CREATIONDATE;
			break;
		case ORDERLASTEDIT:
			ordertype->order = LASTEDIT;
			break;
		case ORDERLIKES:
			ordertype->order = LIKES;
			break;
		case ORDERSIZE:
			ordertype->order = SIZE;
			break;
		case ORDERVIEWS:
			ordertype->order = VIEWS;
			break;
		case ORDERRANDOM:
			ordertype->order = RANDOM;
			break;
	}
	return ordertype;
}


Expression * BinaryExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * exp = calloc(1, sizeof(Expression));
	exp->leftExpression = leftExpression;
	exp->rightExpression = rightExpression;
	exp->type = type;
	return exp;
}

Expression * NegatedExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * exp = calloc(1, sizeof(Expression));
	exp->singleExpression = expression;
	exp->type = OPNOT;
	return exp;
}

Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * exp = calloc(1, sizeof(Expression));
	exp->factor = factor;
	exp->type = FACTOR;
	return exp;
}


Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Factor * TagFactorSemanticAction(Tag * tag){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->tag = tag;
	factor->type = TAG;
	return factor;
}

Factor * MetatagFactorSemanticAction(Metatag * metatag) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->metatag = metatag;
	factor->type = METATAG;
	return factor;
}


Metatag * StringMetatagSemanticAction(char * metatag, String * argument) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Metatag * meta = calloc(1, sizeof(Metatag));
	meta->metatagname = metatag;
	meta->string = argument;
	meta->type = TYPESTRING;
	return meta;
}

Metatag * IntegerMetatagSemanticAction(char * metatag, Integer * argument) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Metatag * meta = calloc(1, sizeof(Metatag));
	meta->metatagname = metatag;
	meta->integer = argument;
	meta->type = TYPEINTEGER;
	return meta;
}

Metatag * DateMetatagSemanticAction(char * metatag, Date * argument) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Metatag * meta = calloc(1, sizeof(Metatag));
	meta->metatagname = metatag;
	meta->date = argument;
	meta->type = TYPEDATE;
	return meta;
}

Metatag * SizeMetatagSemanticAction(char * metatag, SemanticSize * argument) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Metatag * meta = calloc(1, sizeof(Metatag));
	meta->metatagname = metatag;
	meta->size = argument;
	meta->type = TYPESIZE;
	return meta;
}

Metatag * RecallMetatagSemanticAction(String * recallable) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Metatag * meta = calloc(1, sizeof(Metatag));
	meta->string = recallable;
	meta->type = TYPERECALL;
	return meta;
}


String * StringSemanticAction(char * string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	String * str = calloc(1, sizeof(String));
	str->string = string;
	return str;
}


Integer * IntegerSemanticAction(char * integer) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Integer * field = calloc(1, sizeof(Integer));
	field->integer = integer;
	field->quantifier = EQUALS;
	field->fieldtype = UNDEFINEDRANGED;
	return field;
}

Integer * RangedIntegerSemanticAction(char * start, char * end) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Integer * field = calloc(1, sizeof(Integer));
	field->start = start;
	field->end = end;
	field->fieldtype = RANGED;
	return field;
}

Integer * UndefinedRangeIntegerSemanticAction(char * quantifier, char * integer) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Integer * field = calloc(1, sizeof(Integer));
	field->integer = integer;
	field->quantifier = convertToQuantifier(quantifier);
	field->fieldtype = UNDEFINEDRANGED;
	free(quantifier);
	return field;
}

Date * DateSemanticAction(char * date) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Date * field = calloc(1, sizeof(Date));
	field->date = date;
	field->quantifier = EQUALS;
	field->fieldtype = UNDEFINEDRANGED;
	return field;
}

Date * RangedDateSemanticAction(char * start, char * end) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Date * field = calloc(1, sizeof(Date));
	field->start = start;
	field->end = end;
	field->fieldtype = RANGED;
	return field;
}

Date * UndefinedRangeDateSemanticAction(char * quantifier, char * date) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Date * field = calloc(1, sizeof(Date));
	field->date = date;
	field->quantifier = convertToQuantifier(quantifier);
	field->fieldtype = UNDEFINEDRANGED;
	free(quantifier);
	return field;
}


SemanticSize * SizeSemanticAction(char * size) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SemanticSize * field = calloc(1, sizeof(SemanticSize));
	field->size = size;
	field->quantifier = EQUALS;
	field->fieldtype = UNDEFINEDRANGED;
	return field;
}

SemanticSize * RangedSizeSemanticAction(char * start, char * end) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SemanticSize * field = calloc(1, sizeof(SemanticSize));
	field->start = start;
	field->end = end;
	field->fieldtype = RANGED;
	return field;
}

SemanticSize * UndefinedRangeSizeSemanticAction(char * quantifier, char * size) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SemanticSize * field = calloc(1, sizeof(SemanticSize));
	field->size = size;
	field->quantifier = convertToQuantifier(quantifier);
	field->fieldtype = UNDEFINEDRANGED;
	free(quantifier);
	return field;
}


Tag * TagSemanticAction(String * string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Tag * tag = calloc(1, sizeof(Tag));
	tag->tagname = string;
	return tag;
}

Tag * EmptySemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Tag * tag = calloc(1, sizeof(Tag));
	return tag;
}
