#include "SemanticAnalyzer.h"
#include "Symbol.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;
static unsigned int scopeCounter = 0;

void initializeSemanticAnalyzerModule() {
	_logger = createLogger("SemanticAnalyzer");
}

void shutdownSemanticAnalyzerModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */
static boolean _processSubqueries(Subqueries * subqueries);
static boolean _validateExpression(Expression * expression);
static boolean _validateTerm(Term * term);
static boolean _validateBase(Base * base);

static boolean _processSubqueries(Subqueries * subqueries) {
    logDebugging(_logger, "%s", __FUNCTION__);
    if (subqueries == NULL) {
        return true;
    } else {
        if (addPredicate(subqueries->subquery) == false) {
            return false;
        } else {
            return _processSubqueries(subqueries->next);
        }
    }
}

static boolean _validateExpression(Expression * expression) {
    if (expression == NULL) {
        return true;
    }
    pushScope(++scopeCounter);
    if (!_validateTerm(expression->term)) {
        popScope();
        return false;
    } else {
        popScope();
        return _validateExpression(expression->next);
    }
}

static boolean _validateTerm(Term * term) {
    if (term == NULL) {
        return true;
    }
    return _validateBase(term->base) && _validateTerm(term->next);
}

static boolean _validateBase(Base * base) {
    if (base == NULL) {
        return true;
    }
    if (base->factor->type == EXPRESSION) {
        return _validateExpression(base->factor->expression);
    }
    return addBase(base, peekScope());
}

/** PUBLIC FUNCTIONS */

boolean validateAST(Program * program) {
    logDebugging(_logger, "%s", __FUNCTION__);
    if (program == NULL) {
        return false;
    } else if (program->query == NULL){
        return true;
    } else {
        if(_processSubqueries(program->query->subqueries) == false) {
            return false;
        } else {
            return _validateExpression(program->query->mainQuery);
            return true;
        }
    }
}
