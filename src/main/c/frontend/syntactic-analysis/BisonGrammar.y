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
%token <string> STRING				//OK
%token <string> STRMETA				//OK
%token <string> INTMETA				//OK
%token <string> DATEMETA			//OK
%token <string> SIZEMETA			//OK
%token <string> ORDERMETA			//OK
%token <string> INTEGER				//OK
%token <string> DATE				//OK
%token <string> SEMANTICSIZE		//OK


%token <token> CLOSE_PARENTHESIS	//OK
%token <token> OPEN_PARENTHESIS		//OK
%token <token> CLOSE_BRACES			//OK
%token <token> OPEN_BRACES			//OK

%token <token> OR					//OK
%token <token> NOT					//OK
%token <token> AND					//OK

%token <token> WILDCARD				//OK
%token <token> RANGE				//OK
%token <string> QUANTIFIER			//OK

%token <token> DESC					//ok
%token <token> ORDERCREATIONDATE	//OK
%token <token> ORDERLIKES			//OK
%token <token> ORDERVIEWS			//OK
%token <token> ORDERLASTEDIT		//OK
%token <token> ORDERSIZE			//OK
%token <token> ORDERRANDOM			//OK	

%token <token> RECALL				//OK

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

subquery: OPEN_BRACES expression CLOSE_BRACES subqueryname      	{ $$ = SubquerySemanticAction($4, $2); }
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

string: STRING														{ $$ = StringSemanticAction($1); }
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
	;

%%
