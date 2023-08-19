#include "../dot_h/NotExpr.h"
#include "../dot_h/ir_utils.h"

llvm::Value* NotExpr::generate_ir() {
  auto operand_ir {operand->generate_ir()};
  if (!operand_ir || !is_bool(operand_ir))
    return nullptr;
  return builder->CreateNot(operand_ir, "not");
}