#ifndef STATEMENT_H
#define STATEMENT_H

#include "llvm-includes.h"

struct Statement {
  virtual ~Statement() = default;
  virtual llvm::Value* generate_ir() = 0;
};

#endif