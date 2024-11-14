#ifndef SEMANTIC_ANALYZER_HEADER
#define SEMANTIC_ANALYZER_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"


void initializeSemanticAnalyzerModule();
void shutdownSemanticAnalyzerModule();

boolean validateAST(Program * program);

#endif