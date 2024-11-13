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
DESCENDORDER            "_desc"
SPACER                  ([[:blank:]]|\n)
WILDSTRING              {WILD}?{STRING}{WILD}?

WILD                    "*"
RANGE                   ".."
METATAG_SEPARATOR       ":"
QUANT                   (">"|"<"|">="|"<=")

METATAG_SIZE            "size"
METATAG_ORDER           "order"
METATAG_RECALL          "recall"
%%


"("{SPACER}*    					        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
{SPACER}*")"						        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
<INITIAL>"{"{SPACER}*                       { BEGIN(SUBQUERY); isSubquery = true; return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACES); }
<SUBQUERY>{SPACER}*"}"                      { BEGIN(SUBQUERYEND); isSubquery = false; return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACES); }
<SUBQUERYEND>{STRING}                       { return StringLexemeAction(createLexicalAnalyzerContext(), false); }
<SUBQUERYEND>{SPACER}+                      { BEGIN(INITIAL); IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"name"                                      { BEGIN(METATAG_ARG); return MetatagStringLexemeAction(createLexicalAnalyzerContext(), NAME); }
"createdby"                                 { BEGIN(METATAG_ARG); return MetatagStringLexemeAction(createLexicalAnalyzerContext(), CREATED_BY); }
"editedby"                                  { BEGIN(METATAG_ARG); return MetatagStringLexemeAction(createLexicalAnalyzerContext(), EDITED_BY); }
"lasteditedby"                              { BEGIN(METATAG_ARG); return MetatagStringLexemeAction(createLexicalAnalyzerContext(), LASTEDITED_BY); }
"likedby"                                   { BEGIN(METATAG_ARG); return MetatagStringLexemeAction(createLexicalAnalyzerContext(), LIKED_BY); }
"type"                                      { BEGIN(METATAG_ARG); return MetatagStringLexemeAction(createLexicalAnalyzerContext(), FILE_TYPE); }
"pool"                                      { BEGIN(METATAG_ARG); return MetatagStringLexemeAction(createLexicalAnalyzerContext(), POOL); }
"likes"                                     { BEGIN(METATAG_ARG); return MetatagIntegerLexemeAction(createLexicalAnalyzerContext(), LIKES_AMOUNT); }
"views"                                     { BEGIN(METATAG_ARG); return MetatagIntegerLexemeAction(createLexicalAnalyzerContext(), VIEWS_AMOUNT); }
"createdon"                                 { BEGIN(METATAG_ARG); return MetatagDateLexemeAction(createLexicalAnalyzerContext(), CREATED_ON); }
"editedon"                                  { BEGIN(METATAG_ARG); return MetatagDateLexemeAction(createLexicalAnalyzerContext(), EDITED_ON); }
"lasteditedon"                              { BEGIN(METATAG_ARG); return MetatagDateLexemeAction(createLexicalAnalyzerContext(), LASTEDITED_ON); }
{METATAG_SIZE}                              { BEGIN(METATAG_ARG); return MetatagSizeLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_ORDER}                             { BEGIN(METATAG_ARG); return MetatagOrderLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_RECALL}                            { BEGIN(METATAG_ARG); return MetatagRecallLexemeAction(createLexicalAnalyzerContext()); }

{RANGE}                                     { BEGIN(METATAG_ARG); return MetaparameterRangeLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>">"                            { return MetaparameterQuantifierLexemeAction(createLexicalAnalyzerContext(), GREATERTHAN); }
<METATAG_ARG>"<"                            { return MetaparameterQuantifierLexemeAction(createLexicalAnalyzerContext(), LESSERTHAN); }
<METATAG_ARG>">="                           { return MetaparameterQuantifierLexemeAction(createLexicalAnalyzerContext(), GREATEREQUALS); }
<METATAG_ARG>"<="                           { return MetaparameterQuantifierLexemeAction(createLexicalAnalyzerContext(), LESSEREQUALS); }
{DESCENDORDER}                              { return MetaparameterOrderReverseLexemeAction(createLexicalAnalyzerContext()); }

<METATAG_ARG>"creationdate"                 { END_METATAG_CONTEXT; return MetaparameterOrderLexemeAction(createLexicalAnalyzerContext(), ORDER_CREATIONDATE); }
<METATAG_ARG>"lastedited"                   { END_METATAG_CONTEXT; return MetaparameterOrderLexemeAction(createLexicalAnalyzerContext(), ORDER_LASTEDIT); }
<METATAG_ARG>"likes"                        { END_METATAG_CONTEXT; return MetaparameterOrderLexemeAction(createLexicalAnalyzerContext(), ORDER_LIKES); }
<METATAG_ARG>"size"                         { END_METATAG_CONTEXT; return MetaparameterOrderLexemeAction(createLexicalAnalyzerContext(), ORDER_SIZE); }
<METATAG_ARG>"views"                        { END_METATAG_CONTEXT; return MetaparameterOrderLexemeAction(createLexicalAnalyzerContext(), ORDER_VIEWS); }
<METATAG_ARG>"random"                       { END_METATAG_CONTEXT; return MetaparameterOrderLexemeAction(createLexicalAnalyzerContext(), ORDER_RANDOM); }

<METATAG_ARG>"B"                            { END_METATAG_CONTEXT; return MetaparameterSizeQuantifierLexemeAction(createLexicalAnalyzerContext(), BYTE); }
<METATAG_ARG>"KiB"                          { END_METATAG_CONTEXT; return MetaparameterSizeQuantifierLexemeAction(createLexicalAnalyzerContext(), KIB); }
<METATAG_ARG>"MiB"                          { END_METATAG_CONTEXT; return MetaparameterSizeQuantifierLexemeAction(createLexicalAnalyzerContext(), MIB); }
<METATAG_ARG>"GiB"                          { END_METATAG_CONTEXT; return MetaparameterSizeQuantifierLexemeAction(createLexicalAnalyzerContext(), GIB); }


<METATAG_ARG>{INT}                          { END_METATAG_CONTEXT; return MetaparameterIntegerLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{DATETIME}                     { END_METATAG_CONTEXT; return MetaparameterDatetimeLexemeAction(createLexicalAnalyzerContext(), true); }
<METATAG_ARG>{DATE}                         { END_METATAG_CONTEXT; return MetaparameterDatetimeLexemeAction(createLexicalAnalyzerContext(), false); }
<METATAG_ARG>{INT}/{SIZEQUANT}              { return MetaparameterIntegerLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{STRING}                       { END_METATAG_CONTEXT; return MetaparameterStringLexemeAction(createLexicalAnalyzerContext(), false); }
<METATAG_ARG>{WILDSTRING}                   { END_METATAG_CONTEXT; return MetaparameterStringLexemeAction(createLexicalAnalyzerContext(), true); }

{STRING}                                    { return StringLexemeAction(createLexicalAnalyzerContext(), false); }
{WILDSTRING}                                { return StringLexemeAction(createLexicalAnalyzerContext(), true); }

{SPACER}+/{METATAG_ORDER}                   { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

{SPACER}+/({WILDSTRING}|"("|"!")            { return BinaryOperatorLexemeAction(createLexicalAnalyzerContext(), AND); }
{SPACER}+"|"{SPACER}+                       { return BinaryOperatorLexemeAction(createLexicalAnalyzerContext(), OR); }
"!"/({WILDSTRING}|"(")                      { return NOTOperatorLexemeAction(createLexicalAnalyzerContext()); }

{SPACER}+|{METATAG_SEPARATOR}               { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

.									        { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
%%

#include "FlexExport.h"
