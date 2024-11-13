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
SizeType _getSizeQuantifier(char * q);


/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

QuantifierType convertToQuantifier(char * quantifier) {
	if (!strcmp(quantifier, ">=")){
		return GREATEREQUALS;
	} else if (!strcmp(quantifier, ">")){
		return GREATERTHAN;
	} else if (!strcmp(quantifier, "<=")){
		return LESSEREQUALS;
	} else if (!strcmp(quantifier, "<")){
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

Query * OrderProgramSemanticAction(Metaorder * order) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Query * query = calloc(1, sizeof(Query));
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


Ordertypenode * OrdertypeSemanticAction(OrderType order) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Ordertypenode * ordertype = calloc(1, sizeof(Ordertypenode));
	ordertype->order = order;
	return ordertype;
}


Expression * ExpressionSemanticAction(Term * term, Expression * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * exp = calloc(1, sizeof(Expression));
	exp->term = term;
	exp->next = next;
	return exp;
}

Term * TermSemanticAction(Base * base, Term * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Term * term = calloc(1, sizeof(Term));
	term->base = base;
	term->next = next;
	return term;
}

Base * BaseSemanticAction(Factor * factor, boolean negated) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Base * base = calloc(1, sizeof(Base));
	base->factor = factor;
	base->negated = negated;
	return base;
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


Metatag * StringMetatagSemanticAction(MetatagType metatag, String * argument) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Metatag * meta = calloc(1, sizeof(Metatag));
	meta->string = argument;
	meta->type = TYPESTRING;
	meta->metatag = metatag;
	return meta;
}

Metatag * IntegerMetatagSemanticAction(MetatagType metatag, Integer * argument) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Metatag * meta = calloc(1, sizeof(Metatag));
	meta->integer = argument;
	meta->type = TYPEINTEGER;
	meta->metatag = metatag;
	return meta;
}

Metatag * DateMetatagSemanticAction(MetatagType metatag, Date * argument) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Metatag * meta = calloc(1, sizeof(Metatag));
	meta->date = argument;
	meta->type = TYPEDATE;
	meta->metatag = metatag;
	return meta;
}

Metatag * SizeMetatagSemanticAction(SemanticSize * argument) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Metatag * meta = calloc(1, sizeof(Metatag));
	meta->size = argument;
	meta->type = TYPESIZE;
	meta->metatag = FILE_SIZE;
	return meta;
}

Metatag * RecallMetatagSemanticAction(String * recallable) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Metatag * meta = calloc(1, sizeof(Metatag));
	meta->string = recallable;
	meta->type = TYPERECALL;
	meta->metatag = METARECALL;
	return meta;
}


String * StringSemanticAction(char * string, boolean match) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	String * str = calloc(1, sizeof(String));
	str->string = string;
	str->match = (match) ? LIKE : REGULAR;
	return str;
}


Integer * IntegerSemanticAction(int integer) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Integer * field = calloc(1, sizeof(Integer));
	field->integer = integer;
	field->quantifier = EQUALS;
	field->fieldtype = UNDEFINEDRANGED;
	return field;
}

Integer * RangedIntegerSemanticAction(int start, int end) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Integer * field = calloc(1, sizeof(Integer));
	field->start = start;
	field->end = end;
	field->fieldtype = RANGED;
	return field;
}

Integer * UndefinedRangeIntegerSemanticAction(QuantifierType quantifier, int integer) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Integer * field = calloc(1, sizeof(Integer));
	field->integer = integer;
	field->quantifier = quantifier;
	field->fieldtype = UNDEFINEDRANGED;
	return field;
}

Date * DateSemanticAction(char * date, boolean hasTime) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Date * field = calloc(1, sizeof(Date));
	field->date = date;
	field->hasTime = hasTime;
	field->quantifier = EQUALS;
	field->fieldtype = UNDEFINEDRANGED;
	return field;
}

Date * RangedDateSemanticAction(char * start, char * end, boolean hasTime) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Date * field = calloc(1, sizeof(Date));
	field->start = start;
	field->end = end;
	field->hasTime = hasTime;
	field->fieldtype = RANGED;
	return field;
}

Date * UndefinedRangeDateSemanticAction(QuantifierType quantifier, char * date, boolean hasTime) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Date * field = calloc(1, sizeof(Date));
	field->date = date;
	field->quantifier = quantifier;
	field->hasTime = hasTime;
	field->fieldtype = UNDEFINEDRANGED;
	return field;
}


SemanticSize * SizeSemanticAction(int size, SizeType sizequant){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SemanticSize * field = calloc(1, sizeof(SemanticSize));
	field->size = size;
	field->sizequantifier = sizequant;
	field->quantifier = EQUALS;
	field->fieldtype = UNDEFINEDRANGED;
	return field;
}

SemanticSize * RangedSizeSemanticAction(int start, SizeType startquant, int end, SizeType endquant){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SemanticSize * field = calloc(1, sizeof(SemanticSize));
	field->start = start;
	field->end = end;
	field->quantifierstart = startquant;
	field->quantifierend = endquant;
	field->fieldtype = RANGED;
	return field;
}

SemanticSize * UndefinedRangeSizeSemanticAction(QuantifierType quantifier, int size, SizeType sizequant){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SemanticSize * field = calloc(1, sizeof(SemanticSize));
	field->size = size;
	field->sizequantifier = sizequant;
	field->quantifier = quantifier;
	field->fieldtype = UNDEFINEDRANGED;
	return field;
}


Tag * TagSemanticAction(String * string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Tag * tag = calloc(1, sizeof(Tag));
	tag->tagname = string;
	return tag;
}

Query * EmptySemanticAction() {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return NULL;
}

SizeType _getSizeQuantifier(char * q) {
	if (!strcmp(q, "KiB")){
		return KIB;
	} else if (!strcmp(q, "MiB")){
		return MIB;
	} else if (!strcmp(q, "GiB")){
		return GIB;
	} else {
		return BYTE;
	}
}
