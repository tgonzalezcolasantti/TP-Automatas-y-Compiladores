#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * These typedefs allows self-referencing types.
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;
typedef enum OrderType OrderType;
typedef enum FieldType FieldType;
typedef enum MetaType MetaType;
typedef enum QuantifierType QuantifierType;
typedef enum MatchType MatchType;



typedef struct Program Program;
typedef struct Expression Expression;
typedef struct Factor Factor;
typedef struct Query Query;
typedef struct Subqueries Subqueries;
typedef struct Subquery Subquery;
typedef struct Subqueryname Subqueryname;
typedef struct Metaorder Metaorder;
typedef struct Ordertypenode Ordertypenode;
typedef struct Metatag Metatag;
typedef struct String String;
typedef struct Integer Integer;
typedef struct Date Date;
typedef struct SemanticSize SemanticSize;
typedef struct Tag Tag;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum MatchType {
	REGULAR,
	LIKE
};

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
	MatchType match;
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
