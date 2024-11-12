%{

#include "FlexActions.h"
#define END_METATAG_CONTEXT ((isSubquery) ? (BEGIN(SUBQUERY)) : (BEGIN(INITIAL)))
boolean isSubquery = false;
%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%s SUBQUERY
%x SUBQUERYEND
%x METATAG_ARG

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
STRING                  [[:alnum:]]([[:alnum:]]|"-"|"_"|"/"|"("|")")*
INT                     [[:digit:]]+
DATE                    [0-9]{4}"-"("0"[1-9]|"1"[0-2])"-"("0"[1-9]|[1-2][0-9]|"3"[0-1])
DATETIME                {DATE}"T"([0-1][0-9]|"2"[0-3])":"([0-5][0-9])":"([0-5][0-9])"Z"
SIZEQUANT               ("B"|"KiB"|"MiB"|"GiB")
ORDER                   ("creationdate"|"likes"|"views"|"lastedited"|"size"|"random")
DESCENDORDER            "_desc"
SPACER                  ([[:blank:]]|\n)
WILDSTRING              {WILD}?{STRING}{WILD}?

WILD                    "*"
RANGE                   ".."
QUANT                   (">"|"<"|">="|"<=")

METATAG_STRING          ("name"|"createdby"|"editedby"|"lasteditedby"|"likedby"|"type"|"pool")":"
METATAG_INTEGER         ("likes"|"views")":"
METATAG_DATE            ("createdon"|"editedon"|"lasteditedon")":"
METATAG_SIZE            "size:"
METATAG_ORDER           "order:"
METATAG_RECALL          "recall:"
%%


"("{SPACER}*    					        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
{SPACER}*")"						        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
<INITIAL>"{"{SPACER}*                       { BEGIN(SUBQUERY); isSubquery = true; return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACES); }
<SUBQUERY>{SPACER}*"}"                      { BEGIN(SUBQUERYEND); isSubquery = false; return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACES); }
<SUBQUERYEND>{STRING}                       { return StringLexemeAction(createLexicalAnalyzerContext(), false); }
<SUBQUERYEND>{SPACER}+                      { BEGIN(INITIAL); IgnoredLexemeAction(createLexicalAnalyzerContext()); }

{METATAG_STRING}                            { BEGIN(METATAG_ARG); return MetatagStringLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_INTEGER}                           { BEGIN(METATAG_ARG); return MetatagIntegerLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_DATE}                              { BEGIN(METATAG_ARG); return MetatagDateLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_SIZE}                              { BEGIN(METATAG_ARG); return MetatagSizeLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_ORDER}                             { BEGIN(METATAG_ARG); return MetatagOrderLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_RECALL}                            { BEGIN(METATAG_ARG); return MetatagRecallLexemeAction(createLexicalAnalyzerContext()); }
{RANGE}                                     { BEGIN(METATAG_ARG); return MetaparameterRangeLexemeAction(createLexicalAnalyzerContext()); }

<METATAG_ARG>{ORDER}/{DESCENDORDER}         { return MetaparameterOrderLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{DESCENDORDER}                 { END_METATAG_CONTEXT; return MetaparameterOrderReverseLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{ORDER}                        { END_METATAG_CONTEXT; return MetaparameterOrderLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{QUANT}                        { return MetaparameterQuantifierLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{INT}                          { END_METATAG_CONTEXT; return MetaparameterIntegerLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{DATETIME}                     { END_METATAG_CONTEXT; return MetaparameterDatetimeLexemeAction(createLexicalAnalyzerContext(), true); }
<METATAG_ARG>{DATE}                         { END_METATAG_CONTEXT; return MetaparameterDatetimeLexemeAction(createLexicalAnalyzerContext(), false); }

<METATAG_ARG>{SIZEQUANT}                    { END_METATAG_CONTEXT; return MetaparameterSizeLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{INT}/{SIZEQUANT}              { return MetaparameterIntegerLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{STRING}                       { END_METATAG_CONTEXT; return MetaparameterStringLexemeAction(createLexicalAnalyzerContext(), false); }
<METATAG_ARG>{WILDSTRING}                   { END_METATAG_CONTEXT; return MetaparameterStringLexemeAction(createLexicalAnalyzerContext(), true); }

{STRING}                                    { return StringLexemeAction(createLexicalAnalyzerContext(), false); }
{WILDSTRING}                                { return StringLexemeAction(createLexicalAnalyzerContext(), true); }

{SPACER}+/{METATAG_ORDER}                   { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

{SPACER}+/({WILDSTRING}|"("|"!")            { return BinaryOperatorLexemeAction(createLexicalAnalyzerContext(), AND); }
{SPACER}+"|"{SPACER}+                       { return BinaryOperatorLexemeAction(createLexicalAnalyzerContext(), OR); }
"!"/({WILDSTRING}|"(")                      { return NOTOperatorLexemeAction(createLexicalAnalyzerContext()); }

{SPACER}+                                   { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

.									        { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
%%

#include "FlexExport.h"
