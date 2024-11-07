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

static const char * _expressionTypeToCharacter(const ExpressionType type);
static void _generateConstant(const unsigned int indentationLevel, char * constant);
static void _generateEpilogue(void);
static void _generateExpression(const unsigned int indentationLevel, Expression * expression);
static void _generateFactor(const unsigned int indentationLevel, Factor * factor);
static void _generateTag(const unsigned int indentationLevel, Tag * t);
static void _generateMetatag(const unsigned int indentationLevel, Metatag * m);
static void _generateProgram(Program * program);
static void _generateQuery(const unsigned int indentationLevel, Query * q);
static void _generateSubqueries(const unsigned int indentationLevel, Subqueries * s);
static void _generateSubquery(const unsigned int indentationLevel, Subquery * s);
static void _generateSubqueryName(const unsigned int indentationLevel, Subqueryname * n);
static void _generateMetaorder(const unsigned int indentationLevel, Metaorder * m);
static void _generateOrderType(Ordertypenode * o);
static void _generateInteger(const unsigned int indentationLevel, Integer * i);
static void _generateDate(const unsigned int indentationLevel, Date * d);
static void _generateSize(const unsigned int indentationLevel, SemanticSize * s);
static void _generateQuantifier(const unsigned int indentationLevel, QuantifierType q);
static void _generateString(const unsigned int indentationLevel, String * s);
static void _generatePrologue(void);
static char * _indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char * const format, ...);

/**
 * Converts and expression type to its proper SQL operator
 * or returns "F" if that's not possible.
 */
static const char * _expressionTypeToCharacter(const ExpressionType type) {
	switch (type) {
		case OPAND: return "AND";
		case OPOR: return "OR";
		case OPNOT: return "NOT";
		default:
			logError(_logger, "The specified expression type cannot be converted into character: %d", type);
			return "F";
	}
}

/**
 * Generates the output of a constant.
 */
static void _generateConstant(const unsigned int indentationLevel, char * constant) {
	int i = 0;
	while (constant[i]){
		if (constant[i] == '*')
			constant[i] = '%';\
		i++;
	}
	_output(0, "LOWER('%s')", constant);
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
			_output(indentationLevel + 1, "%s ", _expressionTypeToCharacter(expression->type));
			_generateExpression(1 + indentationLevel, expression->rightExpression);
			_output(0, "\n");
			_output(indentationLevel, ")");
			break;
		case OPNOT:
			_generateExpression(1 + indentationLevel, expression->singleExpression);
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
		_generateString(indentationLevel + 1, t->tagname);
}

/**
 * Generates the output of a metatag element.
 */
static void _generateMetatag(const unsigned int indentationLevel, Metatag * m) {
	_output(indentationLevel, "%s", "[ $M$, circle, draw, brown\n");
	_generateConstant(indentationLevel + 1, m->metatagname);
	switch(m->type) {
		case TYPESTRING:
		case TYPERECALL:
			_generateString(indentationLevel + 1, m->string);
			break;
		case TYPEINTEGER:
			_generateInteger(indentationLevel + 1, m->integer);
			break;		
		case TYPEDATE:
			_generateDate(indentationLevel + 1, m->date);
			break;		
		case TYPESIZE:
			_generateSize(indentationLevel + 1, m->size);
			break;			
	}
	_output(indentationLevel, "%s", "]\n");
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
	_generateConstant(indentationLevel + 1, n->name);
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
static void _generateInteger(const unsigned int indentationLevel, Integer * i) {
	_output(indentationLevel, "%s", "[ $I$, circle, draw, black!20\n");
	if (i->fieldtype == RANGED) {
		_generateConstant(indentationLevel + 1, i->start);
		_generateConstant(indentationLevel + 1, i->end);
	} else {
		_generateConstant(indentationLevel + 1, i->integer);
		_generateQuantifier(indentationLevel + 1, i->quantifier);
	}
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a date attribute.
 */
static void _generateDate(const unsigned int indentationLevel, Date * d) {
	_output(indentationLevel, "%s", "[ $D$, circle, draw, black!20\n");
	if (d->fieldtype == RANGED) {
		_generateConstant(indentationLevel + 1, d->start);
		_generateConstant(indentationLevel + 1, d->end);
	} else {
		_generateConstant(indentationLevel + 1, d->date);
		_generateQuantifier(indentationLevel + 1, d->quantifier);
	}
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a size attribute.
 */
static void _generateSize(const unsigned int indentationLevel, SemanticSize * s) {
	_output(indentationLevel, "%s", "[ $S$, circle, draw, black!20\n");
	if (s->fieldtype == RANGED) {
		_generateConstant(indentationLevel + 1, s->start);
		_generateConstant(indentationLevel + 1, s->end);
	} else {
		_generateConstant(indentationLevel + 1, s->size);
		_generateQuantifier(indentationLevel + 1, s->quantifier);
	}
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a string attribute.
 */
static void _generateString(const unsigned int indentationLevel, String * s) {
	if (s->match == LIKE){
		_output(0, "%s", " LIKE ");
	} else {
		_output(0, "%s", "=");
	}
	_generateConstant(indentationLevel + 1, s->string);
}

/**
 * Generates the output of a quantifier.
 */
static void _generateQuantifier(const unsigned int indentationLevel, QuantifierType q) {
	switch(q) {
		case EQUALS:
			_generateConstant(indentationLevel, "=");
			break;
		case GREATERTHAN:
			_generateConstant(indentationLevel, ">");
			break;
		case GREATEREQUALS:
			_generateConstant(indentationLevel, ">=");
			break;
		case LESSERTHAN:
			_generateConstant(indentationLevel, "<");
			break;
		case LESSEREQUALS:
			_generateConstant(indentationLevel, "<=");
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
	"        WHERE favorite.fileID=fileID\n"
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
