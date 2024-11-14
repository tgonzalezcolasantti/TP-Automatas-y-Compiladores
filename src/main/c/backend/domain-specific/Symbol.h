#ifndef SYMBOL_HEADER
#define SYMBOL_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

#define GLOBAL 0

typedef enum SymbolType SymbolType;

typedef struct Symbol Symbol;
typedef struct Scope Scope;

enum SymbolType{
    SYMBOL_PREDICATE,
    SYMBOL_TAG,
    SYMBOL_METATAG
};

struct Symbol{
    SymbolType type;
    int scope;
    union{
        Subquery * predicate;
        struct {
            boolean negated;
            char * name;
            MetatagType metatag;
            FieldType quant;
        };
    };
    Symbol * next;
};

struct Scope {
    int scope;
    Scope * next;
    Scope * prev;
};

void initializeSymbolModule();
void shutdownSymbolModule();

boolean addPredicate(Subquery * predicate);
boolean addBase(Base * base, int scope);

Expression * getPredicate(char * name);

void pushScope(const unsigned int scope);
int popScope();
int peekScope();
int findScope(unsigned int scope);



#endif