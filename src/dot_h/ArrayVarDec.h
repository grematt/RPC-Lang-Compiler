#ifndef ARRAY_VAR_DEC_H
#define ARRAY_VAR_DEC_H

#include "Statement.h"
#include <vector>
#include <string>
class ArrayVarDec : public Statement {
  // vector corresponding to the number of elems in each nested array.
  // ch8[3][5][7] corresponds to vector {3, 5, 7}
  std::vector<int> num_elements;  
  int decl_type;  // the token type of the array (i32_decl_tok, ect)
  std::string name;
  bool is_global;  // true if at a global scope

public:
  ArrayVarDec(int decl_type, std::string name, std::vector<int> num_elements, 
    bool is_global = false) :
    decl_type{decl_type}, name{name}, num_elements{num_elements}, is_global{is_global} {}
  llvm::Value* generate_ir() override;
  llvm::Value* generate_global_ir(llvm::Type* type, std::string name);
};

#endif