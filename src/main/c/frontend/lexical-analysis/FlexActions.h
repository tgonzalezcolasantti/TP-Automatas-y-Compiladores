#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../shared/Environment.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../../shared/Type.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include "LexicalAnalyzerContext.h"
#include <stdio.h>
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeFlexActionsModule();

/** Shutdown module's internal state. */
void shutdownFlexActionsModule();

/**
 * Flex lexeme processing actions.
 */

void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token StringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, boolean match);

Token BinaryOperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);
Token NOTOperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token ParenthesisLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);

Token MetatagSeparatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetatagStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, MetatagType metatag);
Token MetatagIntegerLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, MetatagType metatag);
Token MetatagDateLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, MetatagType metatag);
Token MetatagSizeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetatagOrderLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetatagRecallLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token MetaparameterQuantifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, QuantifierType quantifier);
Token MetaparameterRangeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetaparameterIntegerLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetaparameterDatetimeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, boolean includeDate);
Token MetaparameterSizeQuantifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, SizeType size);
Token MetaparameterStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, boolean match);
Token MetaparameterOrderLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, OrderType order);
Token MetaparameterOrderReverseLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);


Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

#endif
