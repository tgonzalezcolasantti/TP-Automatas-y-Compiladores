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

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

"/*"								        { BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				        { EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+		        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				        { IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"("									        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"									        { return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }

[[:digit:]]+						        { return IntegerLexemeAction(createLexicalAnalyzerContext()); }
[[:alnum:]]([[:alnum:]]|"-"|"_"|"("|")")+   { return StringLexemeAction(createLexicalAnalyzerContext()); }

([[:blank:]]|\n)+						    { return BinaryOperatorLexemeAction(createLexicalAnalyzerContext(), AND); }
([[:blank:]]|\n)+"|"([[:blank:]]|\n)+       { return BinaryOperatorLexemeAction(createLexicalAnalyzerContext(), OR); }
"!"                                         { return NOTOperatorLexemeAction(createLexicalAnalyzerContext()); }

.									        { return UnknownLexemeAction(createLexicalAnalyzerContext()); }
%%

#include "FlexExport.h"
