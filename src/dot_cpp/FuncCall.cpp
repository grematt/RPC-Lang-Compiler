#include "../dot_h/FuncCall.h"
#include "../dot_h/ir_utils.h"

llvm::Value* FuncCall::generate_ir() {
  llvm::Function* function {module->getFunction(func_name)};
  if (!function)
    return post_error_msg("Error: attempt to call undefined function");
  
  size_t args_size {function->arg_size()};
  if (args_size != args.size())
    return post_error_msg("Not enough args");

  // generate IR for function arguments
  std::vector<llvm::Value*> llvm_args;
  for (int i = 0; i < args_size; i++) {
    auto arg_code {args[i]->generate_ir()};
    if (!arg_code)
      return nullptr;

    auto recorded_type {function->getArg(i)->getType()};
    auto arg_type {arg_code->getType()};

    // implicitly cast function arguments from f64 <--> i32/ch8
    if (recorded_type != arg_type)
      if (!(arg_code = type_cast(arg_code, recorded_type, arg_type)))
        return post_error_msg("Parameter type mismatch");

    arg_code = create_array_upcast(recorded_type, arg_type, arg_code);
    if (!arg_code)
      return nullptr;    

    llvm_args.push_back(arg_code);
  }

  return builder->CreateCall(function, llvm_args, "functionCall");
}