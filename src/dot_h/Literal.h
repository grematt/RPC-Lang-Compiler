#ifndef LITERAL_H
#define LITERAL_H

#include "Expression.h"

class Literal : public Expression {
  double val;

public:
  llvm::Value* generate_ir() override;
  Literal(double val, int type) : val{val}, Expression{type} {}
};

#endif