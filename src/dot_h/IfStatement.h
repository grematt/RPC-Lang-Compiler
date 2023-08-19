#ifndef IF_STATEMENT_H
#define IF_STATEMENT_H

#include "Literal.h"
#include <string>
#include <vector>

class IfStatement : public Statement {
  std::unique_ptr<Expression> condition;
  std::vector<std::unique_ptr<Statement>> body, otherwise_body;

public:
  IfStatement(std::unique_ptr<Expression> condition,
    std::vector<std::unique_ptr<Statement>> body, 
    std::vector<std::unique_ptr<Statement>> otherwise_body) :
    condition{std::move(condition)}, body{std::move(body)}, 
    otherwise_body{std::move(otherwise_body)} {}
  llvm::Value* generate_ir() override;
};

#endif