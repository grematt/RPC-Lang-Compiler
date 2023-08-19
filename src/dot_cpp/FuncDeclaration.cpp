#include "../dot_h/FuncDeclaration.h"
#include "../dot_h/ir_utils.h"
#include "../dot_h/tokens.h"

llvm::Function* FuncDeclaration::generate_ir() {
  size_t params_size {params.size()};
  std::vector<llvm::Type*> param_types;
  for (auto& param : params) {
    if (auto type {tok_type_to_llvm_type_obj(param->type, param->get_func_param_arr_sizes())})
      param_types.push_back(type);
    else
      return nullptr;
  }
  
  llvm::Type* ret_type;

  if (return_type == void_ret_tok)
    ret_type = builder->getVoidTy();
  else
    ret_type = tok_type_to_llvm_type_obj(return_type, ret_arr_sizes);

  if (!ret_type)
    return nullptr;

  llvm::FunctionType* func_type {llvm::FunctionType::get(ret_type, param_types, false)};
  llvm::Function* function {llvm::Function::Create(func_type, 
    llvm::Function::ExternalLinkage, fname, module.get())};

  for (int i = 0; i < params_size; i++)
    function->getArg(i)->setName(params[i]->get_name());
  
  return function;
}