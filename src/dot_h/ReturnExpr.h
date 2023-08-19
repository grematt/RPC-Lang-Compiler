#ifndef RETURN_EXPR_H
#define RETURN_EXPR_H

#include "Expression.h"

class ReturnExpr : public Expression {
  std::unique_ptr<Expression> expr;  // value returned

public:
  ReturnExpr(std::unique_ptr<Expression> expr, int type) 
    : expr{std::move(expr)}, Expression{type} {}
  llvm::Value* generate_ir() override;
};

#endif