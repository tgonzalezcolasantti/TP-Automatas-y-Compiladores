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

void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);  //OK

Token StringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);  //OK

Token BinaryOperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token); //OK
Token NOTOperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);                 //OK

Token ParenthesisLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token);    //OK

Token MetatagStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetatagIntegerLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetatagDateLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetatagSizeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetatagOrderLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetatagRecallLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

Token MetaparameterQuantifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetaparameterRangeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetaparameterIntegerLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetaparameterDatetimeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetaparameterSizeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetaparameterStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);
Token MetaparameterOrderLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);


Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext);

#endif
