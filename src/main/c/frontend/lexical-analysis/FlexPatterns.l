%{

#include "FlexActions.h"

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
STRING                  [[:alnum:]]([[:alnum:]]|"-"|"_"|"("|")")*
INT                     [[:digit:]]+
DATE                    [0-9]{4}"-"("0"[1-9]|"1"[0-2])"-"("0"[1-9]|[1-2][0-9]|"3"[0-1])
DATETIME                {DATE}"T"([0-1][0-9]|"2"[0-3])":"([0-5][0-9])":"([0-5][0-9])"Z"
SIZE                    [0-9]+("B"|"KiB"|"MiB"|"GiB")
ORDER                   ("creationdate"|"likes"|"views"|"lastedited"|"size"|"random")"_desc"?
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


"("									        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"									        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
"{"                                         { BEGIN(SUBQUERY); return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACES); }
<SUBQUERY>"}"                               { BEGIN(SUBQUERYEND); return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACES); }
<SUBQUERYEND>{STRING}                       { return StringLexemeAction(createLexicalAnalyzerContext()); }
<SUBQUERYEND>{SPACER}+                      { BEGIN(INITIAL); IgnoredLexemeAction(createLexicalAnalyzerContext()); }

{METATAG_STRING}/({WILDSTRING})             { BEGIN(METATAG_ARG); return MetatagStringLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_INTEGER}/({QUANT}?{INT})           { BEGIN(METATAG_ARG); return MetatagIntegerLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_DATE}/({QUANT}?{DATE})             { BEGIN(METATAG_ARG); return MetatagDateLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_SIZE}/({QUANT}?{SIZE})             { BEGIN(METATAG_ARG); return MetatagSizeLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_ORDER}/{ORDER}                     { BEGIN(METATAG_ARG); return MetatagOrderLexemeAction(createLexicalAnalyzerContext()); }
{METATAG_RECALL}/{STRING}                   { BEGIN(METATAG_ARG); return MetatagRecallLexemeAction(createLexicalAnalyzerContext()); }

<METATAG_ARG>{QUANT}                        { return MetaparameterQuantifierLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{RANGE}                        { return MetaparameterRangeLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{INT}/{RANGE}                  { return MetaparameterIntegerLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{INT}                          { BEGIN(INITIAL); return MetaparameterIntegerLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>({DATE}|{DATETIME})/{RANGE}    { return MetaparameterDatetimeLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>({DATE}|{DATETIME})            { BEGIN(INITIAL); return MetaparameterDatetimeLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{SIZE}/{RANGE}                 { return MetaparameterSizeLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{SIZE}                         { BEGIN(INITIAL); return MetaparameterSizeLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{WILDSTRING}                   { BEGIN(INITIAL); return MetaparameterStringLexemeAction(createLexicalAnalyzerContext()); }
<METATAG_ARG>{ORDER}                        { BEGIN(INITIAL); return MetaparameterOrderLexemeAction(createLexicalAnalyzerContext()); }

{WILDSTRING}                                { return StringLexemeAction(createLexicalAnalyzerContext()); }

{SPACER}+                                   { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

{SPACER}+/({WILDSTRING}|"("|"!")            { return BinaryOperatorLexemeAction(createLexicalAnalyzerContext(), AND); }
{SPACER}+"|"{SPACER}+                       { return BinaryOperatorLexemeAction(createLexicalAnalyzerContext(), OR); }
"!"/({WILDSTRING}|"(")                      { return NOTOperatorLexemeAction(createLexicalAnalyzerContext()); }

.									        { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
%%

#include "FlexExport.h"
