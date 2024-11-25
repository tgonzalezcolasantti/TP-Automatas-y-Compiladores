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

typedef enum ConstantType ConstantType;
typedef enum OrderType OrderType;
typedef enum FieldType FieldType;
typedef enum MetaType MetaType;
typedef enum MetatagType MetatagType;
typedef enum QuantifierType QuantifierType;
typedef enum MatchType MatchType;
typedef enum SizeType SizeType;


typedef struct Program Program;
typedef struct Expression Expression;
typedef struct Term Term;
typedef struct Factor Factor;
typedef struct Constant Constant;
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

enum OrderType {
	ORDER_CREATIONDATE,
	ORDER_LASTEDIT,
	ORDER_LIKES,
	ORDER_SIZE,
	ORDER_VIEWS,
	ORDER_RANDOM
};

enum ConstantType {
	TAG,
	METATAG,
	EXPRESSION
};

enum FieldType {
	RANGED,
	UNDEFINEDRANGED,
	MATCH
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

enum SizeType {
	BYTE,
	KIB,
	MIB,
	GIB
};

enum MetatagType{
	NAME, 
	CREATED_ON,
	CREATED_BY,
	EDITED_BY,
	EDITED_ON, 
	LASTEDITED_BY,
	LASTEDITED_ON,
	LIKED_BY,
	LIKES_AMOUNT,
	FILE_TYPE,
	FILE_SIZE,
	VIEWS_AMOUNT,
	POOL,
	METARECALL
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
	Subqueries * next;
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
	Term * term;
	Expression * next;
};

struct Term {
	Factor * factor;
	Term * next;
};

struct Factor {
	Constant * constant;
	boolean negated;
};

struct Constant {
	union {
		Tag * tag;
		Metatag * metatag;
		Expression * expression;
	};
	ConstantType type;
};

struct Tag {
	String * tagname;
};

struct Metatag {
	union {
		String * string;
		Integer * integer;
		Date * date;
		SemanticSize * size;
	};
	MetaType type;
	MetatagType metatag;
};

struct String {
	char * string;
	MatchType match;
};

struct Integer {
	union{
		struct {
			int integer;
			QuantifierType quantifier;
		};		struct {
			int start;
			int end;
		};
	};
	FieldType fieldtype;
};

struct Date {
	union{
		struct {
			boolean hasTime;
			char * date;
			QuantifierType quantifier;
		};		struct {
			boolean time;
			char * start;
			char * end;
		};
	};
	FieldType fieldtype;
};

struct SemanticSize {
	union{
		struct {
			int size;
			SizeType sizequantifier;
			QuantifierType quantifier;
		};
		struct {
			int start;
			SizeType quantifierstart;
			int end;
			SizeType quantifierend;
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
void releaseTerm(Term * term);
void releaseFactor(Factor * factor);
void releaseConstant(Constant * constant);
void releaseTag(Tag * tag);
void releaseMetatag(Metatag * metatag);
void releaseString(String * str);
void releaseInteger(Integer * integer);
void releaseDate(Date * date);
void releaseSemanticSize(SemanticSize * size);
void releaseProgram(Program * program);
#endif
