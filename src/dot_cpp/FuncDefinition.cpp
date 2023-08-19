#include "../dot_h/FuncDefinition.h"
#include "../dot_h/ir_utils.h"

llvm::Function* FuncDefinition::generate_ir() {
  llvm::Function* function {module->getFunction(declaration->getFName())};
  if (!function)
    function = declaration->generate_ir();  // not previously declared

  if (!function->empty()) {
    return post_error_msg("Multiple definitions or declarations of the same function");
  }

  llvm::BasicBlock* body {llvm::BasicBlock::Create(*context, "body", function)};
  builder->SetInsertPoint(body);

  // clear variable table, and make an entry for the current block
  auto block_name {builder->GetInsertBlock()->getName().str()};
  block_vars.clear();
  
  // allocate memory for the function parameters
  auto& block_vars_index {block_vars[block_name]};
  for (auto& arg : function->args()) {
    auto arg_type {arg.getType()};
    auto var_name {std::string(arg.getName())};

    llvm::AllocaInst* allocated_mem;
    if (arg_type->isArrayTy()) {
      // allocate mem for an array
      allocated_mem = allocate(var_name, arg_type);
      auto gep {create_gep({}, arg_type, allocated_mem, var_name + "-deref")};
      if (!gep)
        return nullptr;
      builder->CreateStore(&arg, gep->second);
    } else {
      // allocate mem for primative
      allocated_mem = allocate(var_name, arg_type);
      builder->CreateStore(&arg, allocated_mem);
    }

    // associate the new mem with the given variable
    block_vars_index[var_name] = allocated_mem;
  }

  // generate the IR for the body
  auto ret_type {function->getReturnType()};
  auto body_ir_gen_result {generate_statement_body(std::move(func_body), ret_type)};

  if (!body_ir_gen_result.first)
    return nullptr;

  // valid return expression in the function
  if (body_ir_gen_result.second) {
    llvm::verifyFunction(*function);
    return function;
  }

  // no return expr in non void func
  if (ret_type->getTypeID() != llvm::Type::VoidTyID)
    return post_error_msg("No return statement specified");

  // no return expr in void func
  builder->CreateRet(llvm::UndefValue::get(function->getReturnType()));
  llvm::verifyFunction(*function);
  return function;
}