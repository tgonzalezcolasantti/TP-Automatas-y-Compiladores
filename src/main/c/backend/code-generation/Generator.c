#include "Generator.h"

/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger * _logger = NULL;

void initializeGeneratorModule() {
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */

static const char _expressionTypeToCharacter(const ExpressionType type);
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
static void _generateOrderType(const unsigned int indentationLevel, Ordertypenode * o);
static void _generateInteger(const unsigned int indentationLevel, Integer * i);
static void _generateDate(const unsigned int indentationLevel, Date * d);
static void _generateSize(const unsigned int indentationLevel, SemanticSize * s);
static void _generateQuantifier(const unsigned int indentationLevel, QuantifierType q);
static void _generateString(const unsigned int indentationLevel, String * s);
static void _generatePrologue(void);
static char * _indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char * const format, ...);

/**
 * Converts and expression type to the proper character of the operation
 * involved, or returns '\0' if that's not possible.
 */
static const char _expressionTypeToCharacter(const ExpressionType type) {
	switch (type) {
		case OPAND: return '+';
		case OPOR: return '|';
		case OPNOT: return '!';
		default:
			logError(_logger, "The specified expression type cannot be converted into character: %d", type);
			return '\0';
	}
}

/**
 * Generates the output of a constant.
 */
static void _generateConstant(const unsigned int indentationLevel, char * constant) {
	_output(1 + indentationLevel, "%s%s%s", "[ $", constant, "$, draw, black]\n");
}

/**
 * Creates the epilogue of the generated output, that is, the final lines that
 * completes a valid Latex document.
 */
static void _generateEpilogue(void) {
	_output(0, "%s",
		"    \\end{forest}\n"
		"\\end{document}\n\n"
	);
}

/**
 * Generates the output of an expression.
 */
static void _generateExpression(const unsigned int indentationLevel, Expression * expression) {
	_output(indentationLevel, "%s", "[ $E$, circle, draw, black!20\n");
	switch (expression->type) {
		case OPAND:
		case OPOR:
			_generateExpression(1 + indentationLevel, expression->leftExpression);
			_output(1 + indentationLevel, "%s%c%s", "[ $", _expressionTypeToCharacter(expression->type), "$, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, expression->rightExpression);
			break;
		case OPNOT:
			_output(1 + indentationLevel, "%s%c%s", "[ $", _expressionTypeToCharacter(expression->type), "$, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, expression->singleExpression);
			break;
		case FACTOR:
			_generateFactor(1 + indentationLevel, expression->factor);
			break;
		default:
			logError(_logger, "The specified expression type is unknown: %d", expression->type);
			break;
	}
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a factor.
 */
static void _generateFactor(const unsigned int indentationLevel, Factor * factor) {
	_output(indentationLevel, "%s", "[ $F$, circle, draw, black!20\n");
	switch (factor->type) {
		case TAG:
			_generateTag(1 + indentationLevel, factor->tag);
			break;
		case METATAG:
			_generateMetatag(1 + indentationLevel, factor->metatag);
			break;
		case EXPRESSION:
			_output(1 + indentationLevel, "%s", "[ $($, circle, draw, purple ]\n");
			_generateExpression(1 + indentationLevel, factor->expression);
			_output(1 + indentationLevel, "%s", "[ $)$, circle, draw, purple ]\n");
			break;
		default:
			logError(_logger, "The specified factor type is unknown: %d", factor->type);
			break;
	}
	_output(indentationLevel, "%s", "]\n");
}

static void _generateTag(const unsigned int indentationLevel, Tag * t) {
	_output(indentationLevel, "%s", "[ $T$, circle, draw, cyan\n");
	if (t->tagname)
		_generateString(indentationLevel + 1, t->tagname);
	_output(indentationLevel, "%s", "]\n");
}

static void _generateMetatag(const unsigned int indentationLevel, Metatag * m) {
	_output(indentationLevel, "%s", "[ $M$, circle, draw, brown\n");
	_generateConstant(indentationLevel + 1, m->metatagname);
	switch(m->type) {
		case TYPESTRING:
		case TYPERECALL:
			_generateString(indentationLevel, m->string);
			break;
		case TYPEINTEGER:
			_generateInteger(indentationLevel, m->integer);
			break;		
		case TYPEDATE:
			_generateDate(indentationLevel, m->date);
			break;		
		case TYPESIZE:
			_generateSize(indentationLevel, m->size);
			break;			
	}
	_output(indentationLevel, "%s", "]\n");
}


/**
 * Generates the output of the program.
 */
static void _generateProgram(Program * program) {			
	_output(2, "%s", "[ $P$, circle, draw, green\n");
	_generateQuery(3, program->query);
	_output(2, "%s", "]\n");
}

static void _generateQuery(const unsigned int indentationLevel, Query * q) {
	_output(indentationLevel, "%s", "[ $Q$, circle, draw, red\n");
	if (q->subqueries){
		_generateSubqueries(indentationLevel + 1, q->subqueries);
	}
	_generateExpression(indentationLevel + 1, q->mainQuery);
	if (q->order){
		_generateMetaorder(indentationLevel + 1, q->order);
	}
	_output(indentationLevel, "%s", "]\n");
}

static void _generateSubqueries(const unsigned int indentationLevel, Subqueries * s) {
	_output(indentationLevel, "%s", "[ $S$, circle, draw, orange\n");
	_generateSubquery(indentationLevel + 1, s->subquery);
	if (s->subqueries) {
		_generateSubqueries(indentationLevel + 1, s->subqueries);
	}
	_output(indentationLevel, "%s", "]\n");
}

static void _generateSubquery(const unsigned int indentationLevel, Subquery * s) {
	_output(indentationLevel, "%s", "[ $sub$, circle, draw, magenta\n");
	_generateExpression(indentationLevel + 1, s->expression);
	_generateSubqueryName(indentationLevel + 1, s->name);
	_output(indentationLevel, "%s", "]\n");
}

static void _generateSubqueryName(const unsigned int indentationLevel, Subqueryname * n) {
	_output(indentationLevel, "%s", "[ $N$, circle, draw, magenta\n");
	_generateConstant(indentationLevel, n->name);
	_output(indentationLevel, "%s", "]\n");
}

static void _generateMetaorder(const unsigned int indentationLevel, Metaorder * m) {
	_output(indentationLevel, "%s", "[ $O$, circle, draw, magenta\n");
	_generateOrderType(indentationLevel + 1, m->order);
	_generateConstant(indentationLevel, (m->desc) ? "DESC" : "ASC");
	_output(indentationLevel, "%s", "]\n");
}

static void _generateOrderType(const unsigned int indentationLevel, Ordertypenode * o) {
	switch(o->order) {
		case CREATIONDATE:
			_generateConstant(indentationLevel, "CREATIONDATE");
			break;
		case LASTEDIT:
			_generateConstant(indentationLevel, "LASTEDIT");
			break;
		case LIKES:
			_generateConstant(indentationLevel, "LIKES");
			break;
		case SIZE:
			_generateConstant(indentationLevel, "SIZE");
			break;
		case VIEWS:
			_generateConstant(indentationLevel, "VIEWS");
			break;
		case RANDOM:
			_generateConstant(indentationLevel, "RANDOM");
			break;
	}
}

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

static void _generateString(const unsigned int indentationLevel, String * s) {
	_output(indentationLevel, "%s", "[ $STR$, circle, draw, teal\n");
	if (s->match == LIKE){
		_output(indentationLevel + 1, "%s", "[ $Match$, circle, draw, black!20]\n");
	} else {
		_output(indentationLevel + 1, "%s", "[ $Exact$, circle, draw, black!20]\n");
	}
	_generateConstant(indentationLevel + 1, s->string);
	_output(indentationLevel, "%s", "]\n");
}

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
 * Creates the prologue of the generated output, a Latex document that renders
 * a tree thanks to the Forest package.
 *
 * @see https://ctan.dcc.uchile.cl/graphics/pgf/contrib/forest/forest-doc.pdf
 */
static void _generatePrologue(void) {
	_output(0, "%s",
		"\\documentclass{standalone}\n\n"
		"\\usepackage[utf8]{inputenc}\n"
		"\\usepackage[T1]{fontenc}\n"
		"\\usepackage{amsmath}\n"
		"\\usepackage{forest}\n"
		"\\usepackage{microtype}\n\n"
		"\\begin{document}\n"
		"    \\centering\n"
		"    \\begin{forest}\n"
	);
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

void generate(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	_generatePrologue();
	_generateProgram(compilerState->abstractSyntaxtTree);
	_generateEpilogue();
	logDebugging(_logger, "Generation is done.");
}
