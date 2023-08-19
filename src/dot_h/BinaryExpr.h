#ifndef BINARY_EXPR_H
#define BINARY_EXPR_H

#include "Expression.h"
#include "tokens.h"

class BinaryExpr : public Expression {
  int op;  // token of operator
  std::unique_ptr<Expression> lhs, rhs;

public:
  llvm::Value* generate_ir() override;
  BinaryExpr(int op, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) 
    : op{op}, lhs{std::move(lhs)}, rhs{std::move(rhs)}, Expression{bin_expr_tok} {}
};

#endif