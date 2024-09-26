%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */
	char* string;
	int integer;
	Token token;

	/** Non-terminals. */

	Program * program;
	Expression * expression;
	Factor * factor;
	Query * query;
	Subqueries * subqueries;
	Subquery * subquery;
	Subqueryname * subqueryname;
	Metaorder * metaorder;
	Ordertypenode * ordertypenode;
	Metatag * metatag;
	String * stringNode;
	Integer * integerNode;
	Date * dateNode;
	SemanticSize * sizeNode;
	Tag * tag;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach. To use this mechanism, the AST must be translated into
 * another structure.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
/*
%destructor { releaseConstant($$); } <constant>
%destructor { releaseExpression($$); } <expression>
%destructor { releaseFactor($$); } <factor>
%destructor { releaseProgram($$); } <program>
*/

/** Terminals. */
%token <string> STRING
%token <string> STRMETA
%token <string> INTMETA
%token <string> DATEMETA
%token <string> SIZEMETA
%token <string> ORDERMETA
%token <string> INTEGER
%token <string> DATE
%token <string> SEMANTICSIZE


%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_BRACES
%token <token> OPEN_BRACES
%token <token> SPLIT

%token <token> OR
%token <token> NOT
%token <token> AND

%token <token> WILDCARD
%token <token> RANGE
%token <string> QUANTIFIER

%token <token> DESC
%token <token> ORDERCREATIONDATE
%token <token> ORDERLIKES
%token <token> ORDERVIEWS
%token <token> ORDERLASTEDIT
%token <token> ORDERSIZE
%token <token> ORDERRANDOM

%token <token> RECALL

%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program
%type <query> query
%type <expression> expression
%type <factor> factor
%type <tag> tag
%type <metatag> metatag
%type <metaorder> metaorder
%type <subquery> subquery
%type <subqueryname> subqueryname
%type <subqueries> subqueries
%type <ordertypenode> ordertype
%type <stringNode> string
%type <integerNode> integer
%type <dateNode> date
%type <sizeNode> size


/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left OR
%left AND
%left NOT


%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.
//query: expression													{ $$ = ExpressionProgramSemanticAction(currentCompilerState(), $1); }
program: query														{ $$ = ProgramSemanticAction(currentCompilerState(), $1); }

query: expression													{ $$ = ExpressionQuerySemanticAction($1); }
	| expression metaorder											{ $$ = ExpressionWithOrderProgramSemanticAction($1, $2); }
	| subqueries expression											{ $$ = ExpressionSubquerySemanticAction($1, $2); }
	| subqueries expression metaorder								{ $$ = ExpressionSubqueryOrderedSemanticAction($1, $2, $3); }
	;

subqueries: subquery												{ $$ = SubquerySingleSemanticAction($1); }
	| subquery subqueries											{ $$ = SubqueryRecursiveSemanticAction($1, $2); }

metaorder: ORDERMETA ordertype										{ $$ = MetaorderSemanticAction($2, false); }
	| ORDERMETA ordertype DESC										{ $$ = MetaorderSemanticAction($2, true); }
	;

ordertype: ORDERCREATIONDATE										{ $$ = OrdertypeSemanticAction($1); }
	| ORDERLASTEDIT													{ $$ = OrdertypeSemanticAction($1); }
	| ORDERLIKES													{ $$ = OrdertypeSemanticAction($1); }
	| ORDERSIZE														{ $$ = OrdertypeSemanticAction($1); }
	| ORDERVIEWS													{ $$ = OrdertypeSemanticAction($1); }
	| ORDERRANDOM													{ $$ = OrdertypeSemanticAction($1); }
	;

subquery: OPEN_BRACES expression CLOSE_BRACES subqueryname SPLIT	{ $$ = SubquerySemanticAction($4, $2); }
;

subqueryname: STRING												{ $$ = SubquerynameSemanticAction($1); }
	;

//expression: expression[left] OR expression[right]					{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
expression: expression[left] OR expression[right]					{ $$ = BinaryExpressionSemanticAction($left, $right, OPOR); }
	| expression[left] AND expression[right]						{ $$ = BinaryExpressionSemanticAction($left, $right, OPAND); }
	| NOT expression[right]											{ $$ = NegatedExpressionSemanticAction($right); }
	| factor														{ $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS				{ $$ = ExpressionFactorSemanticAction($2); }
	| tag															{ $$ = TagFactorSemanticAction($1); }
	| metatag														{ $$ = MetatagFactorSemanticAction($1); }
	;

metatag: STRMETA string												{ $$ = StringMetatagSemanticAction($1, $2); }
	| INTMETA integer												{ $$ = IntegerMetatagSemanticAction($1, $2); }
	| DATEMETA date													{ $$ = DateMetatagSemanticAction($1, $2); }
	| SIZEMETA size													{ $$ = SizeMetatagSemanticAction($1, $2); }
	| RECALL string													{ $$ = RecallMetatagSemanticAction($2); }
	;

string: STRING														{ $$ = StringSemanticAction($1, SINGLE); }
	| WILDCARD STRING												{ $$ = StringSemanticAction($2, SUFFIX); }
	| STRING WILDCARD												{ $$ = StringSemanticAction($1, PREFIX); }
	| WILDCARD STRING WILDCARD										{ $$ = StringSemanticAction($2, INFIX); }
	;

integer: INTEGER													{ $$ = IntegerSemanticAction($1); }
	| INTEGER RANGE INTEGER											{ $$ = RangedIntegerSemanticAction($1, $3); }
	| QUANTIFIER INTEGER											{ $$ = UndefinedRangeIntegerSemanticAction($1, $2); }
	;

date: DATE															{ $$ = DateSemanticAction($1); }
	| DATE RANGE DATE												{ $$ = RangedDateSemanticAction($1, $3); }
	| QUANTIFIER DATE												{ $$ = UndefinedRangeDateSemanticAction($1, $2); }
	;

size: SEMANTICSIZE													{ $$ = SizeSemanticAction($1); }
	| SEMANTICSIZE RANGE SEMANTICSIZE								{ $$ = RangedSizeSemanticAction($1, $3); }
	| QUANTIFIER SEMANTICSIZE										{ $$ = UndefinedRangeSizeSemanticAction($1, $2); }
	;

tag: string															{ $$ = TagSemanticAction($1); }
	|%empty															{ $$ = EmptySemanticAction(); }
	;

%%
