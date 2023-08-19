#include "../dot_h/ReturnExpr.h"
#include "../dot_h/ir_utils.h"
#include "../dot_h/tokens.h"

llvm::Value* ReturnExpr::generate_ir() {
  // returning non nullptr, yet unused, value
  if (type == void_ret_tok) {
    is_return_expr = true;

    // int16 cannot be made anywhere else, this signified a void return
    return llvm::ConstantInt::get(*context, llvm::APInt{16, 0});
  }

  auto expr_ir {expr->generate_ir()};
  if (!expr_ir)
    return nullptr;
  is_return_expr = true;
  return expr_ir;
}