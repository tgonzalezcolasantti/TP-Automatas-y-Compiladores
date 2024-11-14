#include "SemanticAnalyzer.h"
#include "Symbol.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

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
            //TODO more stuff
            return true;
        }
    }
}
