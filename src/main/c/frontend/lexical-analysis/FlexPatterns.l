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
%x MULTILINE_COMMENT
%x METATAG_ARG

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
STRING                  [[:alnum:]]([[:alnum:]]|"-"|"_"|"("|")")+
INT                     [[:digit:]]+
DATE                    ("0"[1-9]|"1"[0-2])"-"("0"[1-9]|[1-2][0-9]|"3"[0-1])"-"[0-9]{4}
DATETIME                {DATE}"T"([0-1][0-9]|"2"[0-3])":"([0-5][0-9])":"([0-5][0-9])"Z"
SIZE                    [0-9]+("B"|"KiB"|"MiB"|"GiB")
ORDER                   ("creationdate"|"likes"|"views"|"lastedited"|"size"|"random")"_desc"?
SPACER                  ([[:blank:]]|\n)

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

"/*"								        { BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				        { EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+		        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"("									        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"									        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
"{"                                         { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACES); }
"}"{SPACER}+                                { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACES); }

{METATAG_STRING}/({WILD}?{STRING})          {}
{METATAG_INTEGER}/({QUANT}?{INT})           {}
{METATAG_DATE}/({QUANT}?{DATE})             {}
{METATAG_SIZE}/({QUANT}?{SIZE})             {}
{METATAG_ORDER}/{ORDER}                     {}
{METATAG_RECALL}/{STRING}                   {}

<METATAG_ARG>{QUANT}                        {}
<METATAG_ARG>{RANGE}                        {}
<METATAG_ARG>{INT}                          {}
<METATAG_ARG>({DATE}|{DATETIME})            {}
<METATAG_ARG>{SIZE}                         {}
<METATAG_ARG>{STRING}                       {}
<METATAG_ARG>{STRING}{WILD}                 {}
<METATAG_ARG>{WILD}{STRING}                 {}
<METATAG_ARG>{WILD}{STRING}{WILD}           {}
<METATAG_ARG>{ORDER}                        {}

{STRING}                                    { return StringLexemeAction(createLexicalAnalyzerContext()); }
{WILD}{STRING}                              {}
{STRING}{WILD}                              {}
{WILD}{STRING}{WILD}                        {}

{SPACER}+                                   { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

{SPACER}+/({STRING}|"(") 		            { return BinaryOperatorLexemeAction(createLexicalAnalyzerContext(), AND); }
{SPACER}+"|"{SPACER}+                       { return BinaryOperatorLexemeAction(createLexicalAnalyzerContext(), OR); }
"!"/({STRING}|"(")                          { return NOTOperatorLexemeAction(createLexicalAnalyzerContext()); }

.									        { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
%%

#include "FlexExport.h"
