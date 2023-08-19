#ifndef NOT_EXPR_H
#define NOT_EXPR_H

#include "Expression.h"
#include "tokens.h"

class NotExpr : public Expression {
  std::unique_ptr<Expression> operand;

public:
  llvm::Value* generate_ir() override;
  NotExpr(std::unique_ptr<Expression> operand) : 
    operand{std::move(operand)},
    Expression{not_tok} {}
};

#endif