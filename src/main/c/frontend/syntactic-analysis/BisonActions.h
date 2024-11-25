#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */

Program * ProgramSemanticAction(CompilerState * compilerState, Query * query);

Query * ExpressionQuerySemanticAction(Expression * expression);
Query * ExpressionWithOrderProgramSemanticAction(Expression * expression, Metaorder * order);
Query * OrderProgramSemanticAction(Metaorder * order);
Query * ExpressionSubquerySemanticAction(Subqueries * subqueries, Expression * expression);
Query * ExpressionSubqueryOrderedSemanticAction(Subqueries * subqueries, Expression * expression, Metaorder * order);

Subqueries * SubquerySingleSemanticAction(Subquery * subquery);
Subqueries * SubqueryRecursiveSemanticAction(Subquery * subquery, Subqueries * remaining);

Subquery * SubquerySemanticAction(Subqueryname * name, Expression * subquery);
Subqueryname * SubquerynameSemanticAction(char * name);

Metaorder * MetaorderSemanticAction(Ordertypenode * order, boolean descending);

Ordertypenode * OrdertypeSemanticAction(OrderType order);

Expression * ExpressionSemanticAction(Term * term, Expression * expression);

Term * TermSemanticAction(Factor * factor, Term * term);
Factor * FactorSemanticAction(Constant * constant, boolean negated);

Constant * ExpressionConstantSemanticAction(Expression * expression);
Constant * TagConstantSemanticAction(Tag * tag);
Constant * MetatagConstantSemanticAction(Metatag * metatag);

Metatag * StringMetatagSemanticAction(MetatagType metatag, String * argument);
Metatag * IntegerMetatagSemanticAction(MetatagType metatag, Integer * argument);
Metatag * DateMetatagSemanticAction(MetatagType metatag, Date * argument);
Metatag * SizeMetatagSemanticAction(SemanticSize * argument);
Metatag * RecallMetatagSemanticAction(String * recallable);

String * StringSemanticAction(char * string, boolean match);

Integer * IntegerSemanticAction(int integer);
Integer * RangedIntegerSemanticAction(int start, int end);
Integer * UndefinedRangeIntegerSemanticAction(QuantifierType quantifier, int integer);

Date * DateSemanticAction(char * date, boolean hasTime);
Date * RangedDateSemanticAction(char * start, char * end, boolean hasTime);
Date * UndefinedRangeDateSemanticAction(QuantifierType quantifier, char * date, boolean hasTime);

SemanticSize * SizeSemanticAction(int size, SizeType sizequant);
SemanticSize * RangedSizeSemanticAction(int start, SizeType startquant, int end, SizeType endquant);
SemanticSize * UndefinedRangeSizeSemanticAction(QuantifierType quantifier, int size, SizeType sizequant);

Tag * TagSemanticAction(String * string);
Query * EmptySemanticAction();

#endif