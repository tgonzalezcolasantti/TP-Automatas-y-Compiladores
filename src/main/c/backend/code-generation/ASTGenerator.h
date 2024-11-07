#ifndef ASTGENERATOR_HEADER
#define ASTGENERATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include <stdarg.h>
#include <stdio.h>

/** Initialize module's internal state. */
void initializeASTGeneratorModule();

/** Shutdown module's internal state. */
void shutdownASTGeneratorModule();

/**
 * Generates the final output using the current compiler state.
 */
void generateAST(CompilerState * compilerState);

#endif
