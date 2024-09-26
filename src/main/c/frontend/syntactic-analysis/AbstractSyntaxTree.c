#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntxTree");
}

void shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PUBLIC FUNCTIONS */

void releaseQuery(Query * query) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (query != NULL) {
		releaseExpression(query->mainQuery);
		if (query->order){
			releaseMetaorder(query->order);
		}		
		if (query->subqueries){
			releaseSubqueries(query->subqueries);
		}
		free(query);
	}
}

void releaseSubqueries(Subqueries * subqueries) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (subqueries != NULL) {
		releaseSubquery(subqueries->subquery);
		if (subqueries->subqueries){
			releaseSubqueries(subqueries->subqueries);
		}		
		free(subqueries);
	}
}

void releaseSubquery(Subquery * subquery) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (subquery != NULL) {
		releaseExpression(subquery->expression);
		releaseSubqueryname(subquery->name);
		free(subquery);
	}
}

void releaseSubqueryname(Subqueryname * subqueryname) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (subqueryname != NULL) {
		free(subqueryname->name);
		free(subqueryname);
	}
}

void releaseMetaorder(Metaorder * metaorder) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (metaorder != NULL) {
		releaseOrdertypenode(metaorder->order);
		free(metaorder);
	}
}

void releaseOrdertypenode(Ordertypenode * ordertypenode) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (ordertypenode != NULL) {
		free(ordertypenode);
	}
}

void releaseExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case OPAND:
			case OPOR:
				releaseExpression(expression->leftExpression);
				releaseExpression(expression->rightExpression);
				break;
			case OPNOT:
				releaseExpression(expression->singleExpression);
			case FACTOR:
				releaseFactor(expression->factor);
				break;
		}
		free(expression);
	}
}

void releaseFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case TAG:
				releaseTag(factor->tag);
				break;			
			case METATAG:
				releaseMetatag(factor->metatag);
				break;
			case EXPRESSION:
				releaseExpression(factor->expression);
				break;
		}
		free(factor);
	}
}

void releaseTag(Tag * tag) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (tag != NULL) {
		releaseString(tag->tagname);
		free(tag);
	}
}

void releaseMetatag(Metatag * metatag){
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (metatag != NULL) {
		switch (metatag->type) {
			case TYPESTRING:
			case TYPERECALL:
				releaseString(metatag->string);
				break;			
			case TYPEINTEGER:
				releaseInteger(metatag->integer);
				break;			
			case TYPEDATE:
				releaseDate(metatag->date);
				break;			
			case TYPESIZE:
				releaseSemanticSize(metatag->size);
				break;
		}		
		free(metatag->metatagname);
		free(metatag);
	}
}

void releaseString(String * str) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (str != NULL) {
		free(str->string);
		free(str);
	}
}


void releaseInteger(Integer * integer) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (integer != NULL) {
		switch (integer->fieldtype) {
			case EXACT:
			case UNDEFINEDRANGED:
				free(integer->integer);
				break;			
			case RANGED:
				free(integer->start);
				free(integer->end);
				break;
		}
		free(integer);
	}
}

void releaseDate(Date * date) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (date != NULL) {
		switch (date->fieldtype) {
			case EXACT:
			case UNDEFINEDRANGED:
				free(date->date);
				break;			
			case RANGED:
				free(date->start);
				free(date->end);
				break;
		}
		free(date);
	}
}

void releaseSemanticSize(SemanticSize * size) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (size != NULL) {
		switch (size->fieldtype) {
			case EXACT:
			case UNDEFINEDRANGED:
				free(size->size);
				break;			
			case RANGED:
				free(size->start);
				free(size->end);
				break;
		}
		free(size);
	}
}