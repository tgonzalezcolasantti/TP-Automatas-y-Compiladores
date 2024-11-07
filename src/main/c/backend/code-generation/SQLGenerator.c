#include "SQLGenerator.h"

/* MODULE INTERNAL STATE */

const static char _indentationCharacter = ' ';
const static char _indentationSize = 4;
static int idcounter = 0;
static Logger * _logger = NULL;

void initializeSQLGeneratorModule() {
	_logger = createLogger("SQL Generator");
}

void shutdownSQLGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */

static const char * _expressionTypeToOperator(const ExpressionType type);
static void _generateConstant(char * constant);														//OK (?)
static void _generateEpilogue(void);
static void _generateExpression(const unsigned int indentationLevel, Expression * expression);		//OK
static void _generateFactor(const unsigned int indentationLevel, Factor * factor);					//OK
static void _generateTag(const unsigned int indentationLevel, Tag * t);								//OK
static void _generateMetatag(const unsigned int indentationLevel, Metatag * m);
static void _generateProgram(Program * program);													//OK
static void _generateQuery(const unsigned int indentationLevel, Query * q);							
static void _generateSubqueries(const unsigned int indentationLevel, Subqueries * s);				
static void _generateSubquery(const unsigned int indentationLevel, Subquery * s);					
static void _generateSubqueryName(const unsigned int indentationLevel, Subqueryname * n);			
static void _generateMetaorder(const unsigned int indentationLevel, Metaorder * m);					//OK
static void _generateOrderType(Ordertypenode * o);													//OK
static void _generateInteger(Integer * i);
static void _generateDate(Date * d);
static void _generateSize(SemanticSize * s);
static void _generateQuantifier(QuantifierType q);
static void _generateString(String * s);															//OK
static void _generatePrologue(void);
static char * _indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char * const format, ...);

/**
 * Converts and expression type to its proper SQL operator
 * or returns "F" if that's not possible.
 */
static const char * _expressionTypeToOperator(const ExpressionType type) {
	switch (type) {
		case OPAND: return "AND";
		case OPOR: return "OR";
		case OPNOT: return "NOT";
		default:
			logError(_logger, "The specified expression type cannot be converted into an operator: %d", type);
			return "F";
	}
}

/**
 * Generates the output of a constant.
 */
static void _generateConstant(char * constant) {
	int i = 0;
	while (constant[i]){
		if (constant[i] == '*')
			constant[i] = '%';\
		i++;
	}
	_output(0, "'%s'", constant);
}

/**
 * Creates the epilogue of the generated output, that is, the final semicolon that
 * completes a proper SQL statement.
 */
static void _generateEpilogue(void) {
	_output(0, "%s",";\n");
}

/**
 * Generates the output of an expression.
 */
static void _generateExpression(const unsigned int indentationLevel, Expression * expression) {
	if (expression->type == OPNOT) {
		_output(0, "NOT ");
	} else {
	}
	switch (expression->type) {
		case FACTOR:
			_generateFactor(indentationLevel, expression->factor);
			break;
		case OPAND:
		case OPOR:
			_output(0, "EXISTS(\n");
			_output(indentationLevel + 1, "%s%d%s", "SELECT * FROM file AS file", idcounter++, "\n");
			_output(indentationLevel + 1, "WHERE ");
			_generateExpression(1 + indentationLevel, expression->leftExpression);
			_output(0, "\n");
			_output(indentationLevel + 1, "%s ", _expressionTypeToOperator(expression->type));
			_generateExpression(1 + indentationLevel, expression->rightExpression);
			_output(0, "\n");
			_output(indentationLevel, ")");
			break;
		case OPNOT:
			_generateExpression(indentationLevel, expression->singleExpression);
			break;
		default:
			logError(_logger, "The specified expression type is unknown: %d", expression->type);
			break;
	}
}

/**
 * Generates the output of a factor.
 */
static void _generateFactor(const unsigned int indentationLevel, Factor * factor) {
	switch (factor->type) {
		case TAG:
			_output(0, "EXISTS(\n");
			_generateTag(indentationLevel + 1, factor->tag);
			_output(0, "\n");
			_output(indentationLevel, ")");
			break;
		case METATAG:
			_output(0, "EXISTS(\n");
			_generateMetatag(indentationLevel + 1, factor->metatag);
			_output(0, "\n");
			_output(indentationLevel, ")");
			break;
		case EXPRESSION:
			_generateExpression(indentationLevel, factor->expression);
			break;
		default:
			logError(_logger, "The specified factor type is unknown: %d", factor->type);
			break;
	}
}

/**
 * Generates the output of a tag element.
 */
static void _generateTag(const unsigned int indentationLevel, Tag * t) {
	_output(indentationLevel, "%s", "SELECT * FROM tag INNER JOIN filetag ON tag.tagID=filetag.tagID\n");
	_output(indentationLevel, "%s", "WHERE filetag.fileID=file.fileID AND tagname");

	if (t->tagname)
		_generateString(t->tagname);
}

/**
 * Generates the output of a metatag element.
 */
static void _generateMetatag(const unsigned int indentationLevel, Metatag * m) {
	if (!strcmp(m->metatagname, "name")){
		_output(indentationLevel, "SELECT * FROM file AS file%d\n", idcounter);
		_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND file%d.filename", idcounter, idcounter);
	} else if (!strcmp(m->metatagname, "createdby")){
		_output(indentationLevel, "SELECT * FROM file AS file%d INNER JOIN appuser AS appuser%d ON appuser%d.userID=file%d.createdby\n", idcounter, idcounter, idcounter, idcounter);
		_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND username", idcounter);
	} else if (!strcmp(m->metatagname, "editedby")){
		_output(indentationLevel, "SELECT * FROM file as file%d\n", idcounter);
		_output(indentationLevel + 1, "INNER JOIN edition AS edition%d ON edition%d.fileID=file%d.fileID\n", idcounter, idcounter, idcounter);
		_output(indentationLevel + 1, "INNER JOIN appuser AS appuser%d ON edition%d.userID=appuser%d.userID \n", idcounter, idcounter, idcounter);
		_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND appuser%d.username", idcounter, idcounter);
	} else if (!strcmp(m->metatagname, "lasteditedby")){
		_output(indentationLevel, "SELECT * FROM file as file%d\n", idcounter);
		_output(indentationLevel + 1, "INNER JOIN (\n");
		_output(indentationLevel + 2, "SELECT DISTINCT ON (fileID) fileID, editiondate, username FROM edition NATURAL JOIN appuser\n");
		_output(indentationLevel + 2, "ORDER BY fileID, editiondate DESC\n");
		_output(indentationLevel + 1, ") as lastEdition%d\n", idcounter);
		_output(indentationLevel + 1, "ON lastEdition%d.fileID=file%d.fileID\n", idcounter, idcounter);		
		_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND lastEdition%d.username", idcounter, idcounter);
	} else if (!strcmp(m->metatagname, "likedby")){
		_output(indentationLevel, "SELECT * FROM file as file%d\n");
		_output(indentationLevel + 1, "INNER JOIN favorite AS favorite%d ON favorite%d.fileID=file%d.fileID\n", idcounter, idcounter, idcounter, idcounter);
		_output(indentationLevel + 1, "INNER JOIN appuser AS appuser%d ON favorite%d.userID=appuser%d.userID\n", idcounter, idcounter, idcounter);	
		_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND appuser%d.username", idcounter, idcounter);
	} else {
		logError(_logger, "The specified metatag cannot be converted into a query: %s", m->metatagname);
		_output(indentationLevel, "SELECT * FROM file");
	}	
	idcounter++;
	switch(m->type) {
		case TYPESTRING:
		case TYPERECALL:
			_generateString(m->string);
			break;
		case TYPEINTEGER:
			_generateInteger(m->integer);
			break;		
		case TYPEDATE:
			_generateDate(m->date);
			break;		
		case TYPESIZE:
			_generateSize(m->size);
			break;			
	}
}

/**
 * Generates the output of the program.
 */
static void _generateProgram(Program * program) {	
	_generateQuery(0, program->query);
}

/**
 * Generates the output of a query object.
 */
static void _generateQuery(const unsigned int indentationLevel, Query * q) {
	if (q->subqueries){
		//_generateSubqueries(indentationLevel + 1, q->subqueries);
	}
	if (q->mainQuery){
		_output(0, "%s", "WHERE ");
		_generateExpression(indentationLevel, q->mainQuery);
	}
	if (q->order){
		_generateMetaorder(indentationLevel, q->order);
	}
}

/**
 * Generates the output of a subquery group.
 */
static void _generateSubqueries(const unsigned int indentationLevel, Subqueries * s) {
	_output(indentationLevel, "%s", "[ $S$, circle, draw, orange\n");
	_generateSubquery(indentationLevel + 1, s->subquery);
	if (s->subqueries) {
		_generateSubqueries(indentationLevel + 1, s->subqueries);
	}
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a subquery.
 */
static void _generateSubquery(const unsigned int indentationLevel, Subquery * s) {
	_output(indentationLevel, "%s", "[ $sub$, circle, draw, magenta\n");
	_generateExpression(indentationLevel + 1, s->expression);
	_generateSubqueryName(indentationLevel + 1, s->name);
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a subquery name.
 */
static void _generateSubqueryName(const unsigned int indentationLevel, Subqueryname * n) {
	_output(indentationLevel, "%s", "[ $N$, circle, draw, magenta\n");
	_generateConstant(n->name);
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a metaorder tag.
 */
static void _generateMetaorder(const unsigned int indentationLevel, Metaorder * m) {
	_output(indentationLevel, "%s", "\nORDER BY ");
	_generateOrderType(m->order);
	_output(0, "%s\n", (m->desc) ? "DESC" : "");
}

/**
 * Generates the output of a metaorder type.
 */
static void _generateOrderType(Ordertypenode * o) {
	switch(o->order) {
		case CREATIONDATE:
			_output(0, "created_on ");
			break;
		case LASTEDIT:
			_output(0, "last_edited_on ");
			break;
		case LIKES:
			_output(0, "likes ");
			break;
		case SIZE:
			_output(0, "size ");
			break;
		case VIEWS:
			_output(0, "views ");
			break;
		case RANDOM:
			_output(0, "RANDOM () ");
			break;
	}
}

/**
 * Generates the output of an integer attribute.
 */
static void _generateInteger(Integer * i) {
	_output(0, "%s", "[ $I$, circle, draw, black!20\n");
	if (i->fieldtype == RANGED) {
		_generateConstant(i->start);
		_generateConstant(i->end);
	} else {
		_generateConstant(i->integer);
		_generateQuantifier(i->quantifier);
	}
	_output(0, "%s", "]\n");
}

/**
 * Generates the output of a date attribute.
 */
static void _generateDate(Date * d) {
	_output(0, "%s", "[ $D$, circle, draw, black!20\n");
	if (d->fieldtype == RANGED) {
		_generateConstant(d->start);
		_generateConstant(d->end);
	} else {
		_generateConstant(d->date);
		_generateQuantifier(d->quantifier);
	}
	_output(0, "%s", "]\n");
}

/**
 * Generates the output of a size attribute.
 */
static void _generateSize(SemanticSize * s) {
	_output(0, "%s", "[ $S$, circle, draw, black!20\n");
	if (s->fieldtype == RANGED) {
		_generateConstant(s->start);
		_generateConstant(s->end);
	} else {
		_generateConstant(s->size);
		_generateQuantifier(s->quantifier);
	}
	_output(0, "%s", "]\n");
}

/**
 * Generates the output of a string attribute.
 */
static void _generateString(String * s) {
	_output(0, "%s", " ILIKE ");
	_generateConstant(s->string);
}

/**
 * Generates the output of a quantifier.
 */
static void _generateQuantifier(QuantifierType q) {
	switch(q) {
		case EQUALS:
			_output(0, "=");
			break;
		case GREATERTHAN:
			_output(0, ">");
			break;
		case GREATEREQUALS:
			_output(0, ">=");
			break;
		case LESSERTHAN:
			_output(0, "<");
			break;
		case LESSEREQUALS:
			_output(0, "<=");
			break;
	}
}


/**
 * Creates the prologue of the generated query, an irrestricted SELECT statement.
 * It will later have restrictions applied based on the given query
 */
static void _generatePrologue(void) {
	_output(0, "%s", "SELECT filename, appuser.username as creator, createdon as created_on,\n"
	"    type, size, views, (\n"
	"        SELECT COUNT(*) as likes FROM favorite\n"
	"        WHERE favorite.fileID=file.fileID\n"
	"    ) as likes, editiondate as last_edited_on, lastEdition.username as last_edited_by\n"
	"FROM file INNER JOIN appuser ON appuser.userID=file.createdby\n"
	"    LEFT OUTER JOIN (\n"
	"        SELECT DISTINCT ON (fileID) fileID, editiondate, username FROM edition NATURAL JOIN appuser\n"
	"        ORDER BY fileID, editiondate DESC\n"
	"    ) as lastEdition\n"	
	"    ON lastEdition.fileID=file.fileID\n");
}

/**
 * Generates an indentation string for the specified level.
 */
static char * _indentation(const unsigned int level) {
	return indentation(_indentationCharacter, level, _indentationSize);
}

/**
 * Outputs a formatted string to standard output. The "fflush" instruction
 * allows to see the output even close to a failure, because it drops the
 * buffering.
 */
static void _output(const unsigned int indentationLevel, const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	char * indentation = _indentation(indentationLevel);
	char * effectiveFormat = concatenate(2, indentation, format);
	vfprintf(stdout, effectiveFormat, arguments);
	fflush(stdout);
	free(effectiveFormat);
	free(indentation);
	va_end(arguments);
}

/** PUBLIC FUNCTIONS */

void generateSQL(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	_generatePrologue();
	_generateProgram(compilerState->abstractSyntaxtTree);
	_generateEpilogue();
	logDebugging(_logger, "Generation is done.");
}
