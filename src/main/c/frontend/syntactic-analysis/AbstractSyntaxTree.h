#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;
typedef enum OrderType OrderType;
typedef enum FieldType FieldType;
typedef enum MetaType MetaType;
typedef enum QuantifierType QuantifierType;



typedef struct Program Program;
typedef struct Expression Expression;		//OK
typedef struct Factor Factor;				//OK
typedef struct Query Query;					//OK
typedef struct Subqueries Subqueries;		//OK
typedef struct Subquery Subquery;			//OK
typedef struct Subqueryname Subqueryname;	//OK
typedef struct Metaorder Metaorder;			//OK
typedef struct Ordertypenode Ordertypenode;	//OK
typedef struct Metatag Metatag;				//OK
typedef struct String String;				//OK
typedef struct Integer Integer;				//OK
typedef struct Date Date;					//OK
typedef struct SemanticSize SemanticSize;	//OK
typedef struct Tag Tag;						//OK

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ExpressionType {
	OPAND,
	OPOR,
	OPNOT,
	FACTOR
};

enum OrderType {
	CREATIONDATE,
	LASTEDIT,
	LIKES,
	SIZE,
	VIEWS,
	RANDOM
};

enum FactorType {
	TAG,
	METATAG,
	EXPRESSION
};

enum FieldType {
	RANGED,
	UNDEFINEDRANGED
};

enum MetaType {
	TYPESTRING,
	TYPEINTEGER,
	TYPEDATE,
	TYPESIZE,
	TYPERECALL
};


enum QuantifierType {
	EQUALS,
	GREATERTHAN,
	GREATEREQUALS,
	LESSERTHAN,
	LESSEREQUALS
};

struct Program {
	Query * query;
};

struct Query {
	Subqueries * subqueries;
	Expression * mainQuery;
	Metaorder * order;
};

struct Subqueries {
	Subquery * subquery;
	Subqueries * subqueries;
};

struct Subquery {
	Expression * expression;
	Subqueryname * name;
};

struct Subqueryname {
	char * name;
};

struct Metaorder{
	Ordertypenode * order;
	boolean desc;
};

struct Ordertypenode{
	OrderType order;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
		Expression * singleExpression;
	};
	ExpressionType type;
};

struct Factor {
	union {
		Tag * tag;
		Metatag * metatag;
		Expression * expression;
	};
	FactorType type;
};

struct Tag {
	String * tagname;
};

struct Metatag {
	char * metatagname;
	union {
		String * string;
		Integer * integer;
		Date * date;
		SemanticSize * size;
	};
	MetaType type;
};

struct String {
	char * string;
};

struct Integer {
	union{
		struct {
			char * integer;
			QuantifierType quantifier;
		};		struct {
			char * start;
			char * end;
		};
	};
	FieldType fieldtype;
};

struct Date {
	union{
		struct {
			char * date;
			QuantifierType quantifier;
		};		struct {
			char * start;
			char * end;
		};
	};
	FieldType fieldtype;
};

struct SemanticSize {
	union{
		struct {
			char * size;
			QuantifierType quantifier;
		};
		struct {
			char * start;
			char * end;
		};
	};
	FieldType fieldtype;
};

/*LEGACY STRUCTS
struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

struct Program {
	Expression * expression;
};

*/

/**
 * Node recursive destructors.
 */

void releaseQuery(Query * query);
void releaseSubqueries(Subqueries * subqueries);
void releaseSubquery(Subquery * subquery);
void releaseSubqueryname(Subqueryname * subqueryname);
void releaseMetaorder(Metaorder * metaorder);
void releaseOrdertypenode(Ordertypenode * ordertypenode);
void releaseExpression(Expression * expression);
void releaseFactor(Factor * factor);
void releaseTag(Tag * tag);
void releaseMetatag(Metatag * metatag);
void releaseString(String * str);
void releaseInteger(Integer * integer);
void releaseDate(Date * date);
void releaseSemanticSize(SemanticSize * size);
void releaseProgram(Program * program);
#endif
