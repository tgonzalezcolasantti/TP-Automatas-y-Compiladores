#include "SQLGenerator.h"
#include "../domain-specific/Symbol.h"

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

static void _generateConstantString(char * constant);												//OK
static void _generateEpilogue(void);																//OK
static void _generateExpression(const unsigned int indentationLevel, Expression * expression);		//OK
static void _generateExpressionRecursive(const unsigned int indentationLevel, Expression * expression);
static void _generateTerm(const unsigned int indentationLevel, Term * term);						//OK
static void _generateTermRecursive(const unsigned int indentationLevel, Term * term);
static void _generateBase(const unsigned int indentationLevel, Base * base);				
static void _generateFactor(const unsigned int indentationLevel, Factor * factor);					//OK
static void _generateTag(const unsigned int indentationLevel, Tag * t);								//OK
static void _generateMetatag(const unsigned int indentationLevel, Metatag * m);						//OK (EXCEPT RECALL)
static void _generateProgram(Program * program);													//OK
static void _generateQuery(const unsigned int indentationLevel, Query * q);							
static void _generateSubqueries(const unsigned int indentationLevel, Subqueries * s);				
static void _generateSubquery(const unsigned int indentationLevel, Subquery * s);					
static void _generateSubqueryName(const unsigned int indentationLevel, Subqueryname * n);			
static void _generateMetaorder(const unsigned int indentationLevel, Metaorder * m);					//OK
static void _generateOrderType(Ordertypenode * o);													//OK
static void _generateInteger(Integer * i);															//OK
static void _generateDate(Date * d);																//OK
static void _generateSize(SemanticSize * s);														//OK
static void _generateQuantifier(QuantifierType q);													//OK
static void _generateString(String * s);															//OK
static void _generatePrologue(void);																//OK
static char * _indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char * const format, ...);
static int _sizetobytes(int size, SizeType type);

static int _sizetobytes(int size, SizeType type){
	switch (type) {
		case GIB: 
			size = size << 10;
		case MIB: 
			size = size << 10;
		case KIB: 
			size = size << 10;
		default:
			return size;
	}
}

/**
 * Generates the output of a constant.
 */
static void _generateConstantString(char * constant) {
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
	if (expression->next){
		_output(0, "EXISTS(\n");
		_output(indentationLevel + 1, "%s%d%s", "SELECT * FROM file AS file", idcounter++, "\n");
		_output(indentationLevel + 1, "WHERE ");
		_generateExpressionRecursive(indentationLevel, expression);
		_output(indentationLevel, ")");
	} else {
		_generateTerm(indentationLevel, expression->term);
	}
}

/**
 * Generates the innards of an expression.
 */
static void _generateExpressionRecursive(const unsigned int indentationLevel, Expression * expression) {
	_generateTerm(1 + indentationLevel, expression->term);
	if (expression->next) {
		_output(0, " OR ");
		_generateExpressionRecursive(indentationLevel, expression->next);
	} else {
		_output(0, "\n");
	}
}

/**
 * Generates the output of a term.
 */
static void _generateTerm(const unsigned int indentationLevel, Term * term) {
	if (term->next) {
		_output(0, "EXISTS(\n");
		_output(indentationLevel + 1, "%s%d%s", "SELECT * FROM file AS file", idcounter++, "\n");
		_output(indentationLevel + 1, "WHERE ");
		_generateTermRecursive(indentationLevel, term);
		_output(indentationLevel, ") ");
	} else {
		_generateBase(indentationLevel, term->base);
	}
}

/**
 * Generates the innards of a term.
 */
static void _generateTermRecursive(const unsigned int indentationLevel, Term * term) {
	_generateBase(1 + indentationLevel, term->base);
	if (term->next) {
		_output(0, " AND ");
		_generateTermRecursive(indentationLevel, term->next);
	} else {
		_output(0, "\n");
	}
}

/**
 * Generates the output of a base.
 */
static void _generateBase(const unsigned int indentationLevel, Base * base) {
	if (base->negated){
		_output(0, "NOT ");
	}
	_generateFactor(indentationLevel, base->factor);
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
	switch (m->metatag){
		case NAME:
			_output(indentationLevel, "SELECT * FROM file AS file%d\n", idcounter);
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND file%d.filename", idcounter, idcounter);
			break;
		case CREATED_BY:
			_output(indentationLevel, "SELECT * FROM file AS file%d INNER JOIN appuser AS appuser%d ON appuser%d.userID=file%d.createdby\n", idcounter, idcounter, idcounter, idcounter);
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND username", idcounter);
			break;
		case EDITED_BY:
			_output(indentationLevel, "SELECT * FROM file As file%d\n", idcounter);
			_output(indentationLevel + 1, "INNER JOIN edition AS edition%d ON edition%d.fileID=file%d.fileID\n", idcounter, idcounter, idcounter);
			_output(indentationLevel + 1, "INNER JOIN appuser AS appuser%d ON edition%d.userID=appuser%d.userID \n", idcounter, idcounter, idcounter);
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND appuser%d.username", idcounter, idcounter);
			break;
		case LASTEDITED_BY:
			_output(indentationLevel, "SELECT * FROM file AS file%d\n", idcounter);
			_output(indentationLevel + 1, "INNER JOIN (\n");
			_output(indentationLevel + 2, "SELECT DISTINCT ON (fileID) fileID, editiondate, username FROM edition NATURAL JOIN appuser\n");
			_output(indentationLevel + 2, "ORDER BY fileID, editiondate DESC\n");
			_output(indentationLevel + 1, ") AS lastEdition%d\n", idcounter);
			_output(indentationLevel + 1, "ON lastEdition%d.fileID=file%d.fileID\n", idcounter, idcounter);		
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND lastEdition%d.username", idcounter, idcounter);
			break;
		case LIKED_BY:
			_output(indentationLevel, "SELECT * FROM file AS file%d\n", idcounter);
			_output(indentationLevel + 1, "INNER JOIN favorite AS favorite%d ON favorite%d.fileID=file%d.fileID\n", idcounter, idcounter, idcounter, idcounter);
			_output(indentationLevel + 1, "INNER JOIN appuser AS appuser%d ON favorite%d.userID=appuser%d.userID\n", idcounter, idcounter, idcounter);	
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND appuser%d.username", idcounter, idcounter);
			break;
		case FILE_TYPE:
			_output(indentationLevel, "SELECT * FROM file AS file%d\n");
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND file%d.type", idcounter, idcounter);
			break;
		case LIKES_AMOUNT:
			_output(indentationLevel, "SELECT file%d.fileID FROM file AS file%d\n", idcounter, idcounter);
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND (\n", idcounter);
			_output(indentationLevel + 1, "SELECT COUNT(*) AS likes FROM favorite AS fav%d\n", idcounter);
			_output(indentationLevel + 1, "WHERE fav%d.fileID=file%d.fileID\n", idcounter, idcounter);
			_output(indentationLevel, ")");
			break;
		case VIEWS_AMOUNT:
			_output(indentationLevel, "SELECT file%d.fileID FROM file AS file%d\n", idcounter, idcounter);
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND file%d.views", idcounter, idcounter);
			break;
		case FILE_SIZE:
			_output(indentationLevel, "SELECT file%d.fileID FROM file AS file%d\n", idcounter, idcounter);
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND file%d.size", idcounter, idcounter);
			break;
		case CREATED_ON:
			_output(indentationLevel, "SELECT file%d.fileID FROM file AS file%d\n", idcounter, idcounter);
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND file%d.createdon", idcounter, idcounter);
			break;
		case EDITED_ON:
			_output(indentationLevel, "SELECT * FROM file As file%d\n", idcounter);
			_output(indentationLevel, "INNER JOIN edition AS edition%d ON edition%d.fileID=file%d.fileID\n", idcounter, idcounter, idcounter);
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND edition%d.editiondate", idcounter, idcounter);
			break;
		case LASTEDITED_ON:
			_output(indentationLevel, "SELECT * FROM file AS file%d\n", idcounter);
			_output(indentationLevel + 1, "INNER JOIN (\n");
			_output(indentationLevel + 2, "SELECT DISTINCT ON (fileID) fileID, editiondate FROM edition\n");
			_output(indentationLevel + 2, "ORDER BY fileID, editiondate DESC\n");
			_output(indentationLevel + 1, ") AS lastEdition%d\n", idcounter);
			_output(indentationLevel + 1, "ON lastEdition%d.fileID=file%d.fileID\n", idcounter, idcounter);		
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND lastEdition%d.editiondate", idcounter, idcounter);
			break;
		case POOL:
			_output(indentationLevel, "SELECT file%d.fileID FROM file AS file%d\n", idcounter, idcounter);
			_output(indentationLevel, "INNER JOIN poolfile AS poolfile%d ON poolfile%d.fileID=file%d.fileID\n", idcounter, idcounter, idcounter);
			_output(indentationLevel, "INNER JOIN pool AS pool%d ON pool%d.poolID=poolfile%d.poolID\n", idcounter, idcounter, idcounter);
			_output(indentationLevel, "WHERE file%d.fileID=file.fileID AND pool%d.poolname", idcounter, idcounter);
			break;
		case METARECALL:
			_output(indentationLevel, "SELECT file%d.fileID FROM file AS file%d\n", idcounter, idcounter);
			_output(indentationLevel, "WHERE ");
			break;
		default:
			logError(_logger, "The specified metatag cannot be converted into a query");
			_output(indentationLevel, "SELECT * FROM file\n");
	}	
	idcounter++;
	switch(m->type) {
		case TYPERECALL:
			_generateExpression(indentationLevel, getPredicate(m->string->string));
		case TYPESTRING:
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
	if (program->query) {
		_generateQuery(0, program->query);
	}
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
	if (s->next) {
		_generateSubqueries(indentationLevel + 1, s->next);
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
	_generateConstantString(n->name);
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
		case ORDER_CREATIONDATE:
			_output(0, "created_on ");
			break;
		case ORDER_LASTEDIT:
			_output(0, "last_edited_on ");
			break;
		case ORDER_LIKES:
			_output(0, "likes ");
			break;
		case ORDER_SIZE:
			_output(0, "size ");
			break;
		case ORDER_VIEWS:
			_output(0, "views ");
			break;
		case ORDER_RANDOM:
			_output(0, "RANDOM () ");
			break;
	}
}

/**
 * Generates the output of an integer attribute.
 */
static void _generateInteger(Integer * i) {
	if (i->fieldtype == RANGED) {
		_output(0, " BETWEEN '%d' AND '%d'", i->start, i->end);
	} else {
		_generateQuantifier(i->quantifier);
		_output(0, "'%d'", i->integer);
	}
}

/**
 * Generates the output of a date attribute.
 */
static void _generateDate(Date * d) {
	if (d->fieldtype == RANGED) {
		if (d->hasTime){
			_output(0, " BETWEEN TIMESTAMP '%s' AND TIMESTAMP '%s'", d->start, d->end);
		} else {
			_output(0, "::DATE BETWEEN DATE '%s' AND DATE '%s'", d->start, d->end);
		}
	} else {
		 if (d->hasTime){
			_output(0, " ");
			_generateQuantifier(d->quantifier);
			_output(0, " DATE '%s'", d->date);
		 } else {
			_output(0, "::DATE ");
			_generateQuantifier(d->quantifier);
			_output(0, " TIMESTAMP '%s'", d->date);
		 }
	}
}

/**
 * Generates the output of a size attribute.
 */
static void _generateSize(SemanticSize * s) {
	if (s->fieldtype == RANGED) {
		_output(0, " BETWEEN '%d' AND '%d'", _sizetobytes(s->start, s->quantifierstart), _sizetobytes(s->end, s->quantifierend));
	} else {
		_generateQuantifier(s->quantifier);
		_output(0, "%d", _sizetobytes(s->size, s->sizequantifier));
	}
}

/**
 * Generates the output of a string attribute.
 */
static void _generateString(String * s) {
	_output(0, "%s", " ILIKE ");
	_generateConstantString(s->string);
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
	_output(0, "%s", "SELECT file.fileID, filename, appuser.username AS creator, createdon AS created_on,\n"
	"    type, size, views, (\n"
	"        SELECT COUNT(*) AS likes FROM favorite\n"
	"        WHERE favorite.fileID=file.fileID\n"
	"    ) AS likes, editiondate AS last_edited_on, lastEdition.username AS last_edited_by\n"
	"FROM file INNER JOIN appuser ON appuser.userID=file.createdby\n"
	"    LEFT OUTER JOIN (\n"
	"        SELECT DISTINCT ON (fileID) fileID, editiondate, username FROM edition NATURAL JOIN appuser\n"
	"        ORDER BY fileID, editiondate DESC\n"
	"    ) AS lastEdition\n"	
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
