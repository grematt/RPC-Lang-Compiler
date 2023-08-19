#ifndef FOR_LOOP_STATEMENT_H
#define FOR_LOOP_STATEMENT_H

#include "Statement.h"

class ForLoopStatement : public Statement {
  // starting paren expression
  std::unique_ptr<Statement> init_statement;
  
  // for loops just use a while loop with a counter
  std::unique_ptr<Statement> while_loop;

public:
  ForLoopStatement(std::unique_ptr<Statement> init_statement,
    std::unique_ptr<Statement> while_loop) :
    init_statement{std::move(init_statement)}, while_loop{std::move(while_loop)} {}
  llvm::Value* generate_ir();
};

#endif