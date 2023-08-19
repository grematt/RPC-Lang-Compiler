#include "../dot_h/AssignmentExpr.h"
#include "../dot_h/ir_utils.h"

llvm::Value* AssignmentExpr::generate_ir() {
  auto parent_name {builder->GetInsertBlock()->getName().str()};

  llvm::Value* lhs_ir;
  llvm::Type* lhs_type;

  // Global LHS -------------------------------------
  if (auto global_var {module->getNamedGlobal(lhs_name)}) {
    auto global_ty {global_var->getValueType()};

    // Array
    if (global_ty->getTypeID() == llvm::Type::ArrayTyID) {
      auto gep {create_gep(std::move(lhs_arr_indices), global_ty, global_var, lhs_name)};

      if (!gep)
        return nullptr;

      lhs_ir = gep->second;
      lhs_type = gep->first;
    } else {
    // Primative
      lhs_ir = global_var;
      lhs_type = global_ty;
    }
  } else {

  // Local LHS --------------------------------------- 
    auto temp {block_vars[parent_name][lhs_name]};
    auto temp_type {temp->getAllocatedType()};

    // Array
    if (temp->isArrayAllocation()) {
      auto gep {create_gep(std::move(lhs_arr_indices), temp_type, temp, lhs_name)};

      if (!gep)
        return nullptr;

      lhs_ir = gep->second;
      lhs_type = gep->first;

    // Primative
    } else {
      lhs_type = temp_type;
      lhs_ir = temp;
    }
  }

  auto rhs_ir {rhs->generate_ir()};
  if (!rhs_ir)
    return nullptr;

  auto rhs_type {rhs_ir->getType()};

  // cast rhs to match type of lhs
  if (rhs_type != lhs_type)  
    rhs_ir = type_cast(rhs_ir, lhs_type, rhs_type);

  if (!lhs_ir || !rhs_ir)
    return nullptr;
    
  builder->CreateStore(rhs_ir, lhs_ir);
  return rhs_ir;
}