#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Statement.h"
struct Expression : Statement {
  int type;  // type from tokens.h

  Expression(int type) : type{type} {}
  virtual ~Expression() = default;
};

#endif