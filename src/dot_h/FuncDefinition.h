#ifndef FUNC_DEFINITION_H
#define FUNC_DEFINITION_H

#include "FuncDeclaration.h"
#include <vector>

class FuncDefinition {
  std::unique_ptr<FuncDeclaration> declaration;
  std::vector<std::unique_ptr<Statement>> func_body;

public:
  llvm::Function* generate_ir();
  FuncDefinition(std::unique_ptr<FuncDeclaration> declaration,
              std::vector<std::unique_ptr<Statement>> func_body) :
    declaration{std::move(declaration)}, func_body{std::move(func_body)} {}
};

#endif