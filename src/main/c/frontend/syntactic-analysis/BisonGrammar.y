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
	MetatagType metatype;
	OrderType ordertype;
	QuantifierType quantifiertype;
	SizeType sizetype;

	/** Non-terminals. */

	Program * program;
	Expression * expression;
	Term * term;
	Base * base;
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
%token <string> STRMATCH			//OK

%token <metatype> STRMETA			//OK
%token <metatype> INTMETA			//OK
%token <metatype> DATEMETA			//OK
%token <metatype> SIZEMETA			//OK
%token <string> ORDERMETA			//OK
%token <integer> INTEGER			//OK
%token <string> DATE				//OK
%token <string> DATETIME			//OK
%token <sizetype> SIZEQUANT			//OK
%token <metatype> RECALL			//OK


%token <token> METATAG_SEPARATOR
%token <token> CLOSE_PARENTHESIS	//OK
%token <token> OPEN_PARENTHESIS		//OK
%token <token> CLOSE_BRACES			//OK
%token <token> OPEN_BRACES			//OK

%token <token> OR					//OK
%token <token> NOT					//OK

%token <token> RANGE				//OK
%token <quantifiertype> QUANTIFIER	//OK

%token <token> DESC					//ok
%token <ordertype> ORDER 			//OK

%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program
%type <query> query
%type <expression> expression
%type <term> term
%type <base> base
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

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.
program: query														{ $$ = ProgramSemanticAction(currentCompilerState(), $1); }

query: expression													{ $$ = ExpressionQuerySemanticAction($1); }
	| expression metaorder											{ $$ = ExpressionWithOrderProgramSemanticAction($1, $2); }
	| metaorder														{ $$ = OrderProgramSemanticAction($1); }
	| subqueries expression											{ $$ = ExpressionSubquerySemanticAction($1, $2); }
	| subqueries expression metaorder								{ $$ = ExpressionSubqueryOrderedSemanticAction($1, $2, $3); }
    | %empty														{ $$ = EmptySemanticAction(); }

	;

subqueries: subquery												{ $$ = SubquerySingleSemanticAction($1); }
	| subquery subqueries											{ $$ = SubqueryRecursiveSemanticAction($1, $2); }

metaorder: ORDERMETA METATAG_SEPARATOR ordertype					{ $$ = MetaorderSemanticAction($3, false); }
	| ORDERMETA METATAG_SEPARATOR ordertype DESC					{ $$ = MetaorderSemanticAction($3, true); }
	;

ordertype: ORDER													{ $$ = OrdertypeSemanticAction($1); }
	;

subquery: OPEN_BRACES expression CLOSE_BRACES subqueryname      	{ $$ = SubquerySemanticAction($4, $2); }
	;

subqueryname: STRING												{ $$ = SubquerynameSemanticAction($1); }
	;

expression: term[left] OR expression[right]							{ $$ = ExpressionSemanticAction($left, $right); }
	| term														    { $$ = ExpressionSemanticAction($1, NULL); }
	;

term: base[left] term[right]										{ $$ = TermSemanticAction($left, $right); }
	| base															{ $$ = TermSemanticAction($1, NULL); }
	;

base: factor														{ $$ = BaseSemanticAction($1, false); }
	| NOT factor													{ $$ = BaseSemanticAction($2, true); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS				{ $$ = ExpressionFactorSemanticAction($2); }
	| tag															{ $$ = TagFactorSemanticAction($1); }
	| metatag														{ $$ = MetatagFactorSemanticAction($1); }
	;

metatag: STRMETA METATAG_SEPARATOR string							{ $$ = StringMetatagSemanticAction($1, $3); }
	| INTMETA METATAG_SEPARATOR integer								{ $$ = IntegerMetatagSemanticAction($1, $3); }
	| DATEMETA METATAG_SEPARATOR date								{ $$ = DateMetatagSemanticAction($1, $3); }
	| SIZEMETA METATAG_SEPARATOR size								{ $$ = SizeMetatagSemanticAction($3); }
	| RECALL METATAG_SEPARATOR string								{ $$ = RecallMetatagSemanticAction($3); }
	;

string: STRING														{ $$ = StringSemanticAction($1, false); }
	| STRMATCH														{ $$ = StringSemanticAction($1, true); }
	;

integer: INTEGER													{ $$ = UndefinedRangeIntegerSemanticAction(EQUALS, $1); }
	| INTEGER RANGE INTEGER											{ $$ = RangedIntegerSemanticAction($1, $3); }
	| QUANTIFIER INTEGER											{ $$ = UndefinedRangeIntegerSemanticAction($1, $2); }
	;

date: DATE															{ $$ = DateSemanticAction($1, false); }
	| DATE RANGE DATE												{ $$ = RangedDateSemanticAction($1, $3, false); }
	| QUANTIFIER DATE												{ $$ = UndefinedRangeDateSemanticAction($1, $2, false); }
	| DATETIME														{ $$ = DateSemanticAction($1, true); }
	| DATETIME RANGE DATETIME										{ $$ = RangedDateSemanticAction($1, $3, true); }
	| QUANTIFIER DATETIME											{ $$ = UndefinedRangeDateSemanticAction($1, $2, true); }
	;

size: INTEGER SIZEQUANT												{ $$ = SizeSemanticAction($1, $2); }
	| INTEGER[il] SIZEQUANT[sl] RANGE INTEGER[ir] SIZEQUANT[sr]		{ $$ = RangedSizeSemanticAction($il, $sl, $ir, $sr); }
	| QUANTIFIER INTEGER SIZEQUANT									{ $$ = UndefinedRangeSizeSemanticAction($1, $2, $3); }
	;

tag: string															{ $$ = TagSemanticAction($1); }
;

%%
