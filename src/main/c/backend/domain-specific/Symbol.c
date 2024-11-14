#include "Symbol.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;
static Symbol * symbolRoot = NULL;

void initializeSymbolModule() {
	_logger = createLogger("SymbolTable");
}

void shutdownSymbolModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */

static Symbol * _getLastSymbol(Symbol * symbol);
static void _appendSymbol(Symbol * symbol);
static Subquery * _searchPredicate(char * name, Symbol * start);
static void _printSymbolTable(Symbol * s);
static void _printSymbolTableRecursive(Symbol * s);


static Symbol * _getLastSymbol(Symbol * symbol) {
    if (!symbol){
        return NULL;
    } else if (!symbol->next) {
        return symbol;
    } else {
        return _getLastSymbol(symbol->next);
    }
}

static void _appendSymbol(Symbol * symbol) {
    Symbol * last = _getLastSymbol(symbolRoot);
    if (last) {
        last->next = symbol;
    } else {
        symbolRoot = symbol;
    }
}

static Subquery * _searchPredicate(char * name, Symbol * start) {
    if (!start) {
        //reached end of list without encountering a duplicate
        return NULL;
    } else {
        if (start->type == PREDICATE) {
            if (!strcmp(start->predicate->name->name, name)){
                return start->predicate;
            }
        }
        return _searchPredicate(name, start->next);
    }
}

static void _printSymbolTable(Symbol * s){
    logDebugging(_logger, "\t\tType\tscope\tname");
    _printSymbolTableRecursive(s);
}

static void _printSymbolTableRecursive(Symbol * s){
    if (!s) {
        return;
    } else {
        switch(s->type) {
            case PREDICATE:
                logDebugging(_logger, "\t\tPredicate\t  %d\t%s", s->scope, s->predicate->name->name);
                break;
            default:
                logCritical(_logger, "Invalid symbol entry found, damn.");
                break;
        }
        _printSymbolTableRecursive(s->next);
    }
}


/** PUBLIC FUNCTIONS */

/**
 * Receives a predicate, and if it can't find it (aka it's unique up until now), adds it to the symbol table
 * Returns true if unique, false if duplicated (and thus should abort parsing upstream)
 */
boolean addPredicate(Subquery * p) {
    logDebugging(_logger, "%s for predicate %s", __FUNCTION__, (p->name->name) ? p->name->name : "_none_");
    if (_searchPredicate(p->name->name, symbolRoot) == NULL){
        Symbol * s = calloc(1, sizeof(Symbol));
        s->predicate = p;
        s->scope = GLOBAL;
        s->type = PREDICATE;
        _appendSymbol(s);
        _printSymbolTable(symbolRoot);
        return true;
    } else {
        logError(_logger, "Predicate '%s' is already declared.", p->name->name);
        _printSymbolTable(symbolRoot);
        return false;
    }
}


/**
 * Retrieves a given predicate (if it exists) from the symbol table
 * Returns the predicate expression, or NULL if it does not exist.
 */
Expression * getPredicate(char * name) {
    Subquery * e = _searchPredicate(name, symbolRoot);
    if (e == NULL) {
        return NULL;
    } else {
        return e->expression;
    }
}