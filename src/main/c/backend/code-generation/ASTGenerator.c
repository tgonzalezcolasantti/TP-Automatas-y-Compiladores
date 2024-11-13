#include "ASTGenerator.h"

/* MODULE INTERNAL STATE */

const static char _indentationCharacter = ' ';
const static char _indentationSize = 4;
static Logger * _logger = NULL;

void initializeASTGeneratorModule() {
	_logger = createLogger("AST Generator");
}

void shutdownASTGeneratorModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */

static const char _expressionTypeToCharacter(const ExpressionType type);
static const char * _sizeTypeToSize(SizeType s);
static void _generateConstantString(const unsigned int indentationLevel, char * constant);
static void _generateConstantInteger(const unsigned int indentationLevel, int constant);
static void _generateEpilogue(void);
static void _generateExpression(const unsigned int indentationLevel, Expression * expression);
static void _generateExpressionRecursive(const unsigned int indentationLevel, Expression * expression);
static void _generateTerm(const unsigned int indentationLevel, Term * term);
static void _generateTermRecursive(const unsigned int indentationLevel, Term * term);
static void _generateBase(const unsigned int indentationLevel, Base * base);
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

static const char * _sizeTypeToSize(SizeType s){
	switch (s){
		case BYTE:
			return "Bytes";
			break;
		case KIB:
			return "KiB";
			break;
		case MIB:
			return "MiB";
			break;
		case GIB:
			return "GiB";
			break;
	}

}

/**
 * Generates the output of a constant.
 */
static void _generateConstantString(const unsigned int indentationLevel, char * constant) {
	_output(indentationLevel, "%s%s%s", "[ \\text{$", constant, "$}, draw, black]\n");
}
static void _generateConstantInteger(const unsigned int indentationLevel, int constant) {
	_output(indentationLevel, "%s%d%s", "[ \\text{$", constant, "$}, draw, black]\n");
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
	_generateExpressionRecursive(indentationLevel, expression);
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the innards of an expression.
 */
static void _generateExpressionRecursive(const unsigned int indentationLevel, Expression * expression) {
	_generateTerm(indentationLevel + 1, expression->term);
	if (expression->next) {
		_output(1 + indentationLevel, "[ $OR$, circle, draw, purple ]\n");
		_generateExpressionRecursive(indentationLevel, expression->next);
	}
}

/**
 * Generates the output of a term.
 */
static void _generateTerm(const unsigned int indentationLevel, Term * term) {
	_output(indentationLevel, "%s", "[ $T$, circle, draw, black!20\n");
	_generateTermRecursive(indentationLevel, term);
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the innards of a term.
 */
static void _generateTermRecursive(const unsigned int indentationLevel, Term * term) {
	_generateBase(indentationLevel + 1, term->base);
	if (term->next) {
		_output(1 + indentationLevel, "[ $AND$, circle, draw, purple ]\n");
		_generateTermRecursive(indentationLevel, term->next);
	}
}

/**
 * Generates the output of a base.
 */
static void _generateBase(const unsigned int indentationLevel, Base * base) {
	_output(indentationLevel, "%s", "[ $B$, circle, draw, black!20\n");
	if (base->negated) {
		_output(1 + indentationLevel, "[ $NOT$, circle, draw, purple ]\n");
	}
	_generateFactor(indentationLevel + 1, base->factor);
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

/**
 * Generates the output of a tag element.
 */
static void _generateTag(const unsigned int indentationLevel, Tag * t) {
	_output(indentationLevel, "%s", "[ $TG$, circle, draw, cyan\n");
	if (t->tagname)
		_generateString(indentationLevel + 1, t->tagname);
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a metatag element.
 */
static void _generateMetatag(const unsigned int indentationLevel, Metatag * m) {
	_output(indentationLevel, "%s", "[ $M$, circle, draw, brown\n");
	_generateConstantString(indentationLevel + 1, m->metatagname);
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
	_output(2, "%s", "[ $P$, circle, draw, green\n");
	if (program->query){
		_generateQuery(3, program->query);
	}
	_output(2, "%s", "]\n");
}

/**
 * Generates the output of a query object.
 */
static void _generateQuery(const unsigned int indentationLevel, Query * q) {
	_output(indentationLevel, "%s", "[ $Q$, circle, draw, red\n");
	if (q->subqueries){
		_generateSubqueries(indentationLevel + 1, q->subqueries);
	}
	if (q->mainQuery){
		_generateExpression(indentationLevel + 1, q->mainQuery);
	}
	if (q->order){
		_generateMetaorder(indentationLevel + 1, q->order);
	}
	_output(indentationLevel, "%s", "]\n");
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
	_generateConstantString(indentationLevel + 1, n->name);
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a metaorder tag.
 */
static void _generateMetaorder(const unsigned int indentationLevel, Metaorder * m) {
	_output(indentationLevel, "%s", "[ $O$, circle, draw, magenta\n");
	_generateOrderType(indentationLevel + 1, m->order);
	_generateConstantString(indentationLevel + 1, (m->desc) ? "DESC" : "ASC");
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a metaorder type.
 */
static void _generateOrderType(const unsigned int indentationLevel, Ordertypenode * o) {
	switch(o->order) {
		case ORDER_CREATIONDATE:
			_generateConstantString(indentationLevel, "CREATIONDATE");
			break;
		case ORDER_LASTEDIT:
			_generateConstantString(indentationLevel, "LASTEDIT");
			break;
		case ORDER_LIKES:
			_generateConstantString(indentationLevel, "LIKES");
			break;
		case ORDER_SIZE:
			_generateConstantString(indentationLevel, "SIZE");
			break;
		case ORDER_VIEWS:
			_generateConstantString(indentationLevel, "VIEWS");
			break;
		case ORDER_RANDOM:
			_generateConstantString(indentationLevel, "RANDOM");
			break;
	}
}

/**
 * Generates the output of an integer attribute.
 */
static void _generateInteger(const unsigned int indentationLevel, Integer * i) {
	_output(indentationLevel, "%s", "[ $I$, circle, draw, black!20\n");
	if (i->fieldtype == RANGED) {
		_generateConstantInteger(indentationLevel + 1, i->start);
		_generateConstantInteger(indentationLevel + 1, i->end);
	} else {
		_generateConstantInteger(indentationLevel + 1, i->integer);
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
		_generateConstantString(indentationLevel + 1, d->start);
		_generateConstantString(indentationLevel + 1, d->end);
	} else {
		_generateConstantString(indentationLevel + 1, d->date);
		_generateQuantifier(indentationLevel + 1, d->quantifier);
	}
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a size attribute.
 */
static void _generateSize(const unsigned int indentationLevel, SemanticSize * s) {
	if (s->fieldtype == RANGED) {
		_output(indentationLevel + 1, "%s", "[ $start$, circle, draw, black!20\n");
		_output(indentationLevel + 2, "%s%d%s", "[ \\text{$", s->start, "$}, draw, black]\n");
		_output(indentationLevel + 2, "%s%s%s", "[ \\text{$", _sizeTypeToSize(s->quantifierstart), "$}, draw, black]\n");
		_output(indentationLevel, "%s", "]\n");
		_output(indentationLevel + 1, "%s", "[ $end$, circle, draw, black!20\n");
		_output(indentationLevel + 2, "%s%d%s", "[ \\text{$", s->end, "$}, draw, black]\n");
		_output(indentationLevel + 2, "%s%s%s", "[ \\text{$", _sizeTypeToSize(s->quantifierend), "$}, draw, black]\n");
		_output(indentationLevel, "%s", "]\n");
	} else {
		_generateQuantifier(indentationLevel + 1, s->quantifier);
		_output(indentationLevel + 1, "%s%d%s", "[ \\text{$", s->size, "$}, draw, black]\n");
		_output(indentationLevel + 1, "%s%s%s", "[ \\text{$", _sizeTypeToSize(s->sizequantifier), "$}, draw, black]\n");
		_output(indentationLevel, "%s", "]\n");
	}
}

/**
 * Generates the output of a string attribute.
 */
static void _generateString(const unsigned int indentationLevel, String * s) {
	_output(indentationLevel, "%s", "[ $STR$, circle, draw, teal\n");
	if (s->match == LIKE){
		_output(indentationLevel + 1, "%s", "[ $Match$, circle, draw, black!20]\n");
	} else {
		_output(indentationLevel + 1, "%s", "[ $Exact$, circle, draw, black!20]\n");
	}
	_generateConstantString(indentationLevel + 1, s->string);
	_output(indentationLevel, "%s", "]\n");
}

/**
 * Generates the output of a quantifier.
 */
static void _generateQuantifier(const unsigned int indentationLevel, QuantifierType q) {
	switch(q) {
		case EQUALS:
			_generateConstantString(indentationLevel, "=");
			break;
		case GREATERTHAN:
			_generateConstantString(indentationLevel, ">");
			break;
		case GREATEREQUALS:
			_generateConstantString(indentationLevel, ">=");
			break;
		case LESSERTHAN:
			_generateConstantString(indentationLevel, "<");
			break;
		case LESSEREQUALS:
			_generateConstantString(indentationLevel, "<=");
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

void generateAST(CompilerState * compilerState) {
	logDebugging(_logger, "Generating final output...");
	_generatePrologue();
	_generateProgram(compilerState->abstractSyntaxtTree);
	_generateEpilogue();
	logDebugging(_logger, "Generation is done.");
}
