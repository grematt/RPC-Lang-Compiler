#ifndef PRIMATIVE_VAR_DEC
#define PRIMATIVE_VAR_DEC

#include "Expression.h"
#include <string>

class PrimativeVarDec : public Statement {
  int decl_type;  // declared type token
  std::string name;
  std::unique_ptr<Expression> init_val;
  bool is_global;  // true if scope is global

public:
  PrimativeVarDec(int decl_type, std::string name, std::unique_ptr<Expression> 
    init_val, bool is_global = false) :
    decl_type{decl_type}, name{name}, init_val{std::move(init_val)}, 
    is_global{is_global} {}
  llvm::Value* generate_ir() override;
  llvm::Value* generate_global_ir();
};

#endif