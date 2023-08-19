#ifndef VAR_ID_H
#define VAR_ID_H

#include "Expression.h"
#include <string>
#include <vector>

class VarId : public Expression {
  std::string name;
  // order of array dereferences
  std::vector<std::unique_ptr<Expression>> arr_indices;
  // sizes of arrays/nested arrays when var is passed to a function.
  // these must be integer values so the above instance var isn't used for this
  std::vector<int> func_param_arr_sizes;  

public:
  llvm::Value* generate_ir() override;
  VarId(std::string& name, int type, 
    std::vector<std::unique_ptr<Expression>> arr_indices = {}) : 
    name{name}, Expression{type}, arr_indices{std::move(arr_indices)} {}
  VarId(std::string& name, int type, std::vector<int> func_param_arr_sizes) : 
    name{name}, Expression{type}, arr_indices{}, 
    func_param_arr_sizes{func_param_arr_sizes} {}
  std::vector<int>& get_func_param_arr_sizes();
  std::string& get_name();
};

#endif