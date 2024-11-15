#include "Symbol.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;
static Symbol * symbolRoot = NULL;
static Scope * scopeStack = NULL;

void initializeSymbolModule() {
	_logger = createLogger("SymbolTable");
}

void shutdownSymbolModule() {
	if (_logger != NULL) {
        while (symbolRoot != NULL) {
            Symbol * temp = symbolRoot;
            symbolRoot = symbolRoot->next;
            free(temp);
        }
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */

static Symbol * _getLastSymbol(Symbol * start);
static Scope * _getLastScope(Scope * start);
static void _appendSymbol(Symbol * symbol);
static Subquery * _searchPredicate(char * name, Symbol * start);
static void _printSymbolTable(Symbol * s);
static void _printSymbolTableRecursive(Symbol * s);
static boolean _addBaseRec(Base * base, unsigned int scope, Symbol * start);
static boolean _checkDuplicateSymbol(Symbol * new, Symbol * start);

static Symbol * _getLastSymbol(Symbol * start) {
    if (!start){
        return NULL;
    } else if (!start->next) {
        return start;
    } else {
        return _getLastSymbol(start->next);
    }
}

static void _appendSymbol(Symbol * symbol) {
    logDebugging(_logger, "%s", __FUNCTION__);
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
        if (start->type == SYMBOL_PREDICATE) {
            if (!strcmp(start->predicate->name->name, name)){
                return start->predicate;
            }
        }
        return _searchPredicate(name, start->next);
    }
}

static void _printSymbolTable(Symbol * s){
    logDebugging(_logger, "---------------------------------------");
    logDebugging(_logger, "SYMBOLS\tType\t\tscope\t name");
    _printSymbolTableRecursive(s);
}

static void _printScopeTable() {
    logDebugging(_logger, "SCOPES");
    Scope * current = _getLastScope(scopeStack);
    if (current == NULL) {
        logDebugging(_logger, "\t(no active scopes)");
    } else {
        logDebugging(_logger, "\t->%d", current->scope);
        current = current->prev;
        while (current != NULL) {
            logDebugging(_logger, "\t  %d", current->scope);
            current = current->prev;
        }
    }

}

static void _printSymbolTableRecursive(Symbol * s){
    if (!s) {
        return;
    } else {
        switch(s->type) {
            case SYMBOL_PREDICATE:
                logDebugging(_logger, "\t\tPredicate\t  %d\t %s", s->scope, s->predicate->name->name);
                break;
            case SYMBOL_TAG:
                logDebugging(_logger, "\t\tTag\t\t  %d\t%c%s", s->scope, s->negated ? '!' : ' ', s->name);
                break;
            case SYMBOL_METATAG:
                logDebugging(_logger, "\t\tMetatag\t\t  %d\t%c%d:%s", s->scope, s->negated ? '!' : ' ', s->metatag, (s->quant == RANGED) ? "range" : (s->quant == MATCH) ? "str" : "quant");
                break;
            default:
                logCritical(_logger, "Invalid symbol entry found, damn.");
                break;
        }
        _printSymbolTableRecursive(s->next);
    }
}

static Scope * _getLastScope(Scope * start) {
    if (start == NULL){
        return NULL;
    } else if (start->next == NULL) {
        return start;
    } else {
        return _getLastScope(start->next);
    }
}

static Symbol * _createBaseSymbol(Base * base, unsigned int scope) {
    logDebugging(_logger, "%s", __FUNCTION__);
    Symbol * new = calloc(1, sizeof(Symbol));
    new->negated = base->negated;
    new->scope = scope;
    switch(base->factor->type) {
        case TAG:
            new->type = SYMBOL_TAG;
            new->name = base->factor->tag->tagname->string;
            return new;
        case METATAG:
            new->type = SYMBOL_METATAG;
            new->metatag = base->factor->metatag->metatag;
            switch(base->factor->metatag->type) {
                case TYPESTRING:
                case TYPERECALL:
                    new->quant = MATCH;
                    new->name = base->factor->metatag->string->string;
                    break;
                case TYPEINTEGER:
                    new->quant = base->factor->metatag->integer->fieldtype;
                    break;                
                case TYPEDATE:
                    new->quant = base->factor->metatag->date->fieldtype;
                    break;                
                case TYPESIZE:
                    new->quant = base->factor->metatag->size->fieldtype;
                    break;      
            }
            return new;
        default:
            logCritical(_logger, "Attempting to parse an invalid base for symbol table");
            free(new);
            return NULL;
    }
}

static boolean _addBaseRec(Base * base, unsigned int scope, Symbol * start){
    Symbol * new = _createBaseSymbol(base, scope);
    if (!_checkDuplicateSymbol(new, start)) {
        if (start->next == NULL) {
            start->next = new;
            return true;
        } else {
            return _addBaseRec(base, scope, start->next);
        }
    }
    logWarning(_logger, "Duplicate symbol within scope found for:");
    _printSymbolTableRecursive(new);
    _printSymbolTable(symbolRoot);
    _printScopeTable(scopeStack);
    free(new);
    return false;
}

/**
 * Returns true if a given symbol is already logged in the symbol table with an active scope
 * Or false if its unique up until now.
 */
static boolean _checkDuplicateSymbol(Symbol * new, Symbol * start) {
    if (start == NULL) {
        return false;
    }
    if (new->type == start->type){
        switch(new->type) {
            case SYMBOL_PREDICATE:
                if (!strcmp(new->predicate->name->name, start->predicate->name->name)) {
                    return true;
                }
                break;
            case SYMBOL_TAG:
                if (!strcmp(new->name, start->name)) {
                    if (findScope(start->scope) >= 0) {
                        //aka the duplicated tag's scopes overlap
                        return true;
                    }
                }
                break;
            case SYMBOL_METATAG:
                if (new->metatag == start->metatag) {
                    if (new->metatag == METARECALL && strcmp(new->name, start->name)) {
                        //different recall metatags -> we allow this
                        return false;
                    }
                    if (findScope(start->scope) >= 0) {
                        //aka the duplicated tag's scopes overlap
                        return true;
                    }
                }
                break;
            default:
                logError(_logger, "An invalid symbol was checked for duplicates.");
                return true;
        }
    }
    return _checkDuplicateSymbol(new, start->next);
}

/** PUBLIC FUNCTIONS */

/**
 * Receives a predicate, and if it can't find it (aka it's unique up until now), adds it to the symbol table
 * Returns true if unique, false if duplicated (and thus should abort parsing upstream)
 */
boolean addPredicate(Subquery * p) {
    logDebugging(_logger, "%s for predicate '%s'", __FUNCTION__, (p->name->name) ? p->name->name : "_none_");
    if (_searchPredicate(p->name->name, symbolRoot) == NULL){
        Symbol * s = calloc(1, sizeof(Symbol));
        s->predicate = p;
        s->scope = GLOBAL;
        s->type = SYMBOL_PREDICATE;
        _appendSymbol(s);
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

/**
 * adds a base (aka tag or metatag) to the symbol table if it has not been added already
 * bases with expressions are not evaluated in this instance.
 * returns true if added successfully (and thus unique up until now), or false if duplicated.
 */
boolean addBase(Base * base, int scope) {
    logDebugging(_logger, "%s", __FUNCTION__);
    if (symbolRoot == NULL) {
        symbolRoot = _createBaseSymbol(base, scope);
        return true;
    } else {
        return _addBaseRec(base, scope, symbolRoot);
    }
}


/**
 * Pushes a scope onto the scope stack
 * Useful for determining duplicate tags n' stuff
 */
void pushScope(const unsigned int scope) {
    logDebugging(_logger, "%s %d", __FUNCTION__, scope);
    Scope * myScope = calloc(1, sizeof(Scope));
    myScope->scope = scope;
    Scope * last = _getLastScope(scopeStack);
    if (last == NULL) {
        scopeStack = myScope;
    } else {
        last->next = myScope;
        myScope->prev = last;
    }
}

/**
 * Removes a scope from the scope stack
 * And returns its value
 */
int popScope() {
    logDebugging(_logger, "%s %d", __FUNCTION__, peekScope());
    int ans = -1;
    if (scopeStack == NULL) {
        logCritical(_logger, "Attempted to pop an empty scope stack.");
    } else if (scopeStack->next == NULL) {
        ans = scopeStack->scope;
        free(scopeStack);
        scopeStack = NULL;
    } else {
        Scope * last = _getLastScope(scopeStack);
        ans = last->scope;
        last->prev->next = NULL;
        free(last);
    }
    return ans;
}

/**
 * Returns the value of the last scope in the stack without removing it
 */
int peekScope() {
    Scope * last = _getLastScope(scopeStack);
    if (last == NULL) {
        return -1;
    } else {
        return last->scope;
    }
}

/**
 * Returns how far away from the current scope the given scope is
 * Or returns -1 if given scope is not active rn. 
 */
int findScope(unsigned int scope) {
    logDebugging(_logger, "%s", __FUNCTION__);
    Scope * last = _getLastScope(scopeStack);
    for (int i = 0; last != NULL; i++, last = last->prev){
        if (last->scope == scope) {
            return i;
        }
    }
    return -1;
}