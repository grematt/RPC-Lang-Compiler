#include "../dot_h/ArrayVarDec.h"
#include "../dot_h/parser_utils.h"
#include "../dot_h/ir_utils.h"

llvm::Value* ArrayVarDec::generate_global_ir(llvm::Type* type, std::string name) {
  module->getOrInsertGlobal(name, type);
  auto global_var {module->getNamedGlobal(name)};
  
  // init global array to all 0
  global_var->setInitializer(llvm::ConstantAggregateZero::get(type));
  return global_var;
}

llvm::Value* ArrayVarDec::generate_ir() {
  auto elem_type {tok_type_to_llvm_type_obj(arr_tok_to_from_prim_tok(decl_type))};
  if (!elem_type)
    return nullptr;

  auto type {create_arr_type(num_elements, elem_type)};
  if (!type)
    return nullptr;

  if (is_global)
    return generate_global_ir(type, name);

  // get current variable table
  std::string instert_location {builder->GetInsertBlock()->getName().str()};
  if (block_vars[instert_location][name] != nullptr ||
      module->getNamedGlobal(name) != nullptr)
    return post_error_msg("Attempt to redefine existing variable");

  auto allocated_mem {allocate(name, type)};

  // insert var in the variable table of the current block
  auto& variables {block_vars[instert_location]};
  variables[name] = allocated_mem;
  return allocated_mem;
}