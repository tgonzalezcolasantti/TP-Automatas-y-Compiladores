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

Token MetatagStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, MetatagType metatag) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->metatype = metatag;
	return STRMETA;
}

Token MetatagIntegerLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, MetatagType metatag) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->metatype = metatag;
	return INTMETA;
}

Token MetatagDateLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, MetatagType metatag) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->metatype = metatag;
	return DATEMETA;
}

Token MetatagSizeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->metatype = FILE_SIZE;
	return SIZEMETA;
}

Token MetatagOrderLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	return ORDERMETA;
}

Token MetatagRecallLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->metatype = METARECALL;
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
	lexicalAnalyzerContext->semanticValue->integer = atoi(lexicalAnalyzerContext->lexeme);
	return INTEGER;
}

Token MetaparameterDatetimeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, boolean includeTime) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = lexicalAnalyzerContext->lexeme;
	if (includeTime){
		return DATETIME;
	}
	return DATE;
}

Token MetaparameterSizeLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = lexicalAnalyzerContext->lexeme;
	return SIZEQUANT;
}

Token MetaparameterStringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, boolean match) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = lexicalAnalyzerContext->lexeme;
	if (match){
		return STRMATCH;
	} else {
		return STRING;
	}}

Token MetaparameterOrderLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	if (strcmp(lexicalAnalyzerContext->lexeme, "likes") == 0){
		return ORDERLIKES;
	} else if (strcmp(lexicalAnalyzerContext->lexeme, "creationdate") == 0){
		return ORDERCREATIONDATE;
	} else if (strcmp(lexicalAnalyzerContext->lexeme, "random") == 0){
		return ORDERRANDOM;
	} else if (strcmp(lexicalAnalyzerContext->lexeme, "lastedited") == 0){
		return ORDERLASTEDIT;
	} else if (strcmp(lexicalAnalyzerContext->lexeme, "size") == 0){
		return ORDERSIZE;
	} else if (strcmp(lexicalAnalyzerContext->lexeme, "views") == 0){
		return ORDERVIEWS;
	} 
	return ORDERRANDOM;
}

Token MetaparameterOrderReverseLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	return DESC;
}

Token StringLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, boolean match) {
	_logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
	lexicalAnalyzerContext->semanticValue->string = lexicalAnalyzerContext->lexeme;
	if (match){
		return STRMATCH;
	} else {
		return STRING;
	}
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
