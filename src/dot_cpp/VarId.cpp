#include "../dot_h/VarId.h"
#include "../dot_h/ir_utils.h"

llvm::Value* VarId::generate_ir() {
  // global variable ---------------
  if (auto global_var {module->getNamedGlobal(name)}) {
    auto global_type {global_var->getValueType()};
    
    //global array
    if (global_type->isArrayTy()) {  // array
      if (auto gep {create_gep(std::move(arr_indices), global_type, global_var, name)})
        return builder->CreateLoad(gep->first, gep->second, name);
      return nullptr;
    }

    // global primative
    return builder->CreateLoad(global_type, global_var, name);
  }

  // local variable -----------------

  auto parent_block {builder->GetInsertBlock()->getName().str()};
  auto var {block_vars[parent_block][name]};

  if (!var)
    return post_error_msg("Variable not present in scope");

  // local array
  if (var->isArrayAllocation()) {
    auto arr_type {var->getAllocatedType()};
    if (auto gep {create_gep(std::move(arr_indices), arr_type, var, name)})
      return builder->CreateLoad(gep->first, gep->second, name);
    return nullptr;
  }

  // local primative
  return builder->CreateLoad(var->getAllocatedType(), var, name);
}

std::vector<int>& VarId::get_func_param_arr_sizes() {
  return func_param_arr_sizes;
}

std::string& VarId::get_name() {
  return name;
}