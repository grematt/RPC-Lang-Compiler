#ifndef WHILE_LOOP_STATEMENT_H
#define WHILE_LOOP_STATEMENT_H

#include "Literal.h"
#include <vector>

class WhileLoopStatement : public Statement {
  std::unique_ptr<Expression> condition;
  std::vector<std::unique_ptr<Statement>> body;

public:
  WhileLoopStatement(std::unique_ptr<Expression> condition,
    std::vector<std::unique_ptr<Statement>> body) :
    condition{std::move(condition)}, body{std::move(body)} {}
  llvm::Value* generate_ir() override;
};

#endif