#include "FlexActions.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;
static boolean _logIgnoredLexemes = true;

void initializeFlexActionsModule() {
	_logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
	_logger = createLogger("FlexActions");
}

void shutdownFlexActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/* PRIVATE FUNCTIONS */

static void _logLexicalAnalyzerContext(const char * functionName, LexicalAnalyzerContext * lexicalAnalyzerContext);

/**
 * Logs a lexical-analyzer context in DEBUGGING level.
 */
static void _logLexicalAnalyzerContext(const char * functionName, LexicalAnalyzerContext * lexicalAnalyzerContext) {
	char * escapedLexeme = escape(lexicalAnalyzerContext->lexeme);
	logDebugging(_logger, "%s: %s (context = %d, length = %d, line = %d)",
		functionName,
		escapedLexeme,
		lexicalAnalyzerContext->currentContext,
		lexicalAnalyzerContext->length,
		lexicalAnalyzerContext->line);
	free(escapedLexeme);
}

unsigned int trim(const LexicalAnalyzerContext * context) {
	int i;
	for (i = context->length; i && context->lexeme[i] && context->lexeme[i] != ' '; i--){
	}
	return i;
}

/* PUBLIC FUNCTIONS */

void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	if (_logIgnoredLexemes) {
		_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	}
}

Token ArithmeticOperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	return token;
}

Token MetatagStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	return STRMETA;
}

Token MetatagIntegerLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	return INTMETA;
}

Token MetatagDateLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	return DATEMETA;
}

Token MetatagSizeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	return SIZEMETA;
}

Token MetatagOrderLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	return ORDERMETA;
}

Token MetatagRecallLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	return RECALL;
}

Token MetaparameterQuantifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	return QUANTIFIER;
}

Token MetaparameterRangeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	return RANGE;
}

Token MetaparameterIntegerLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	return INTEGER;
}

Token MetaparameterDatetimeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	return DATE;
}

Token MetaparameterSizeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	return SEMANTICSIZE;
}

Token MetaparameterStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = malloc(lexicalAnalyzerContext->length + 1);
	strncpy(lexicalAnalyzerContext->semanticValue->string, lexicalAnalyzerContext->lexeme, lexicalAnalyzerContext->length);
	lexicalAnalyzerContext->semanticValue->string[lexicalAnalyzerContext->length] = 0;
	return STRING;
}

Token MetaparameterOrderLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	return ORDERRANDOM;
}


Token StringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	int trimmed = trim(lexicalAnalyzerContext);
	char * copy = malloc(trimmed + 1);
	strncpy(copy, lexicalAnalyzerContext->lexeme, trimmed);
	copy[trimmed] = 0;
	lexicalAnalyzerContext->semanticValue->string = copy;
	return STRING;
}

Token BinaryOperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	return token;
}

Token NOTOperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	return NOT;
}

Token ParenthesisLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->token = token;
	return token;
}

Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	return UNKNOWN;
}
