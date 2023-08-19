#include "../dot_h/ConstantArray.h"
#include <vector>
#include "../dot_h/ir_utils.h"
#include "../dot_h/tokens.h"

llvm::Value* ConstantArray::generate_ir() {
  std::vector<llvm::Constant*> ir_vals;

  for (auto& val : vals) {
    auto val_ir {val->generate_ir()};
    if (!val_ir)
      return nullptr;

    // all values should be literals, which are llvm::constant
    auto casted {llvm::dyn_cast<llvm::Constant>(val_ir)};
    if (!casted)
      return nullptr;

    ir_vals.push_back(casted);
  }

  auto elem_type_obj {tok_type_to_llvm_type_obj(elem_type)};
  if (!elem_type_obj)
    return nullptr;

  auto arr_ty {llvm::ArrayType::get(elem_type_obj, ir_vals.size())};
  
  return llvm::ConstantArray::get(arr_ty, ir_vals);
}