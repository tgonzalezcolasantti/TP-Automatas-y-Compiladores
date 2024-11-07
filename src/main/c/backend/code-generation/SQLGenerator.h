#ifndef SQLGENERATOR_HEADER
#define SQLGENERATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include <stdarg.h>
#include <stdio.h>

/** Initialize module's internal state. */
void initializeSQLGeneratorModule();

/** Shutdown module's internal state. */
void shutdownSQLGeneratorModule();

/**
 * Generates the final output using the current compiler state.
 */
void generateSQL(CompilerState * compilerState);

#endif
