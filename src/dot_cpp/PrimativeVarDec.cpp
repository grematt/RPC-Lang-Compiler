#include "../dot_h/PrimativeVarDec.h"
#include "../dot_h/tokens.h"
#include "../dot_h/ir_utils.h"

llvm::Value* PrimativeVarDec::generate_global_ir() {
  int bits {};
  
  if (module->getNamedGlobal(name))
    return post_error_msg("Attempt to redefine existing global var");

  // create a new global
  switch (decl_type) {
    case i32_decl_tok:
      module->getOrInsertGlobal(name, builder->getInt32Ty());
      break;
    case ch8_decl_tok:
      module->getOrInsertGlobal(name, builder->getInt8Ty());
      bits = 8;
      break;
    case f64_decl_tok:
      module->getOrInsertGlobal(name, builder->getDoubleTy());
      break;
    default:
      return nullptr;
  }
  
  // get global we just created
  auto global_var {module->getNamedGlobal(name)}; 

  auto init_val_ir {init_val->generate_ir()};

  auto global_type {global_var->getValueType()};
  auto init_type {init_val_ir->getType()};

  if (global_type != init_type)
    if (!(init_val_ir = type_cast(init_val_ir, global_type, init_type)))
      return nullptr;

  auto init_data {llvm::dyn_cast<llvm::ConstantData>(init_val_ir)};
  if (!init_data)
    return post_error_msg("Global varaibles must be initalized with a literal value");

  global_var->setInitializer(init_data);

  return global_var;
}

llvm::Value* PrimativeVarDec::generate_ir() {
  // VarId::generate_ir() is not used in order to avoid an extra load
  // instruction. We only need to store rhs into lhs, we do not need 
  // to load lhs and then store. Hence some logic in VarId::generate_ir()
  // is reused

  if (is_global)
    return generate_global_ir();

  std::string instert_location {builder->GetInsertBlock()->getName().str()};
  if (block_vars[instert_location][name] != nullptr ||
      module->getNamedGlobal(name) != nullptr)
    return post_error_msg("Attempt to redefine existing variable");

  llvm::AllocaInst* allocated_mem;
  int bits {};
  
  switch (decl_type) {
    case i32_decl_tok:
      allocated_mem = allocate(name, builder->getInt32Ty());
      break;
    case ch8_decl_tok:
      allocated_mem = allocate(name, builder->getInt8Ty());
      bits = 8;
      break;
    case f64_decl_tok:
      allocated_mem = allocate(name, builder->getDoubleTy());
      break;
    default:
      return nullptr;
  }
  
  auto init_val_ir {init_val->generate_ir()};
  if (!init_val_ir)
    return nullptr;

  auto temp_ty {init_val_ir->getType()};
    
  auto llvm_type {allocated_mem->getAllocatedType()};
  if (llvm_type != temp_ty) 
    if (!(init_val_ir = type_cast(init_val_ir, llvm_type, temp_ty)))
      return nullptr;

  builder->CreateStore(init_val_ir, allocated_mem);

  auto& variables {block_vars[instert_location]};
  variables[name] = allocated_mem;
  return init_val_ir;
}