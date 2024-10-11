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
Query * ExpressionSubquerySemanticAction(Subqueries * subqueries, Expression * expression);
Query * ExpressionSubqueryOrderedSemanticAction(Subqueries * subqueries, Expression * expression, Metaorder * order);

Subqueries * SubquerySingleSemanticAction(Subquery * subquery);
Subqueries * SubqueryRecursiveSemanticAction(Subquery * subquery, Subqueries * remaining);

Subquery * SubquerySemanticAction(Subqueryname * name, Expression * subquery);
Subqueryname * SubquerynameSemanticAction(char * name);

Metaorder * MetaorderSemanticAction(Ordertypenode * order, boolean descending);

Ordertypenode * OrdertypeSemanticAction(Token order);

Expression * BinaryExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * NegatedExpressionSemanticAction(Expression * expression);
Expression * FactorExpressionSemanticAction(Factor * factor);

Factor * ExpressionFactorSemanticAction(Expression * expression);
Factor * TagFactorSemanticAction(Tag * tag);
Factor * MetatagFactorSemanticAction(Metatag * metatag);

Metatag * StringMetatagSemanticAction(char * metatag, String * argument);
Metatag * IntegerMetatagSemanticAction(char * metatag, Integer * argument);
Metatag * DateMetatagSemanticAction(char * metatag, Date * argument);
Metatag * SizeMetatagSemanticAction(char * metatag, SemanticSize * argument);
Metatag * RecallMetatagSemanticAction(char * metatag, String * recallable);

String * StringSemanticAction(char * string, boolean match);

Integer * IntegerSemanticAction(char * integer);
Integer * RangedIntegerSemanticAction(char * start, char * end);
Integer * UndefinedRangeIntegerSemanticAction(char * quantifier, char * integer);

Date * DateSemanticAction(char * date);
Date * RangedDateSemanticAction(char * start, char * end);
Date * UndefinedRangeDateSemanticAction(char * quantifier, char * date);

SemanticSize * SizeSemanticAction(char * size);
SemanticSize * RangedSizeSemanticAction(char * start, char * end);
SemanticSize * UndefinedRangeSizeSemanticAction(char * quantifier, char * size);

Tag * TagSemanticAction(String * string);
Query * EmptySemanticAction();

#endif