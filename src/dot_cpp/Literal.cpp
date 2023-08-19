#include "../dot_h/Literal.h"
#include "../dot_h/tokens.h"
#include "../dot_h/ir_utils.h"

llvm::Value* Literal::generate_ir() {
  // double -> uint64_t narrowing convention (suppressed with parens),
  // not a problem from what I can tell, 2^31 - 1 can still be converted to f64,
  // though of course accuracy is lost
  switch(type) {
    case i32_tok:
      return llvm::ConstantInt::get(*context, llvm::APInt(32, val, true));
    case f64_tok:
      return llvm::ConstantFP::get(*context, llvm::APFloat(val));
    case ch8_tok:
      return llvm::ConstantInt::get(*context, llvm::APInt(8, val, true));
    default:
      return post_error_msg("Invalid type");
  }
}