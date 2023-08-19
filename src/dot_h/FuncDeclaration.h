#ifndef FUNC_DECLARATION_H 
#define FUNC_DECLARATION_H 

#include "VarId.h"
#include <string>
#include <vector>

class FuncDeclaration {
  std::string fname;
  std::vector<std::unique_ptr<VarId>> params;
  int return_type; // return type token
  // sizes of each nested array in return type, if return type is an array
  std::vector<int> ret_arr_sizes;

public:
  llvm::Function* generate_ir();

  FuncDeclaration(std::string& fname, int return_type,
    std::vector<std::unique_ptr<VarId>> params, std::vector<int> ret_arr_sizes = {}) :
    fname{fname}, return_type{return_type}, params{std::move(params)}, 
    ret_arr_sizes{ret_arr_sizes} {}

  const std::string& getFName() {
    return fname;
  }
};

#endif