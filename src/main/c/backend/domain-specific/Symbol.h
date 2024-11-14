#ifndef SYMBOL_HEADER
#define SYMBOL_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

#define GLOBAL 0

typedef enum SymbolType SymbolType;

typedef struct Symbol Symbol;

enum SymbolType{
    PREDICATE,
    BASE
};

struct Symbol{
    SymbolType type;
    int scope;
    union{
        Subquery * predicate;
        Base * base;
    };
    Symbol * next;
};

void initializeSymbolModule();
void shutdownSymbolModule();

boolean addPredicate(Subquery * predicate);
boolean addBase(Base * base, boolean negated, int scope);

Expression * getPredicate(char * name);


#endif