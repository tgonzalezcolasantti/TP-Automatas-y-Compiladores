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
void releaseProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		if (program->query) {
			releaseQuery(program->query);
		}
		free(program);
	}
}

void releaseQuery(Query * query) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (query != NULL) {
		if (query->mainQuery){
			releaseExpression(query->mainQuery);
		}
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
		if (subqueries->next){
			releaseSubqueries(subqueries->next);
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
		if (expression->next){
			releaseExpression(expression->next);
		}
		releaseTerm(expression->term);
		free(expression);
	}
}

void releaseTerm(Term * term) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (term != NULL) {
		if (term->next){
			releaseTerm(term->next);
		}
		releaseFactor(term->factor);
		free(term);
	}
}

void releaseFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		releaseConstant(factor->constant);
		free(factor);
	}
}

void releaseConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		switch (constant->type) {
			case TAG:
				releaseTag(constant->tag);
				break;			
			case METATAG:
				releaseMetatag(constant->metatag);
				break;
			case EXPRESSION:
				releaseExpression(constant->expression);
				break;
		}
		free(constant);
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
		free(integer);
	}
}

void releaseDate(Date * date) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (date != NULL) {
		switch (date->fieldtype) {
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
	free(size);
}