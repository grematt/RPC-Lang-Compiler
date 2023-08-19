#include "../dot_h/BinaryExpr.h"
#include "../dot_h/tokens.h"
#include "../dot_h/ir_utils.h"

llvm::Value* BinaryExpr::generate_ir() {
  auto lhs_ir {lhs->generate_ir()};
  auto rhs_ir {rhs->generate_ir()};

  if (!lhs_ir || !rhs_ir)
    return nullptr;

  auto lhs_type {lhs_ir->getType()};
  auto rhs_type {rhs_ir->getType()};

  // both types must be ints, or both types must be doubles
  auto llvm_double {builder->getDoubleTy()};
  if (lhs_type == llvm_double && rhs_type != llvm_double)
    rhs_ir = type_cast(rhs_ir, lhs_type, rhs_type);
  else if (rhs_type == llvm_double && lhs_type != llvm_double)
    lhs_ir = type_cast(lhs_ir, rhs_type, lhs_type);
  
  // could be removed and put in above conditionals
  if (!lhs_ir || !rhs_ir)
    return nullptr;

  if (lhs_type == llvm_double || rhs_type == llvm_double)
    switch (op) {
      case add_tok:
        return builder->CreateFAdd(lhs_ir, rhs_ir, "fadd");
      case sub_tok:
        return builder->CreateFSub(lhs_ir, rhs_ir, "fsub");
      case div_tok:
        return builder->CreateFDiv(lhs_ir, rhs_ir, "fdiv");
      case multi_tok:
        return builder->CreateFMul(lhs_ir, rhs_ir, "fmulti");
      case less_tok:
        return builder->CreateIntCast(builder->CreateFCmpOLT(lhs_ir, rhs_ir, "flthan"),
          llvm::Type::getInt1Ty(*context), false);
      case less_eq_tok:
        return builder->CreateIntCast(builder->CreateFCmpOLE(lhs_ir, rhs_ir, "feqLthan"),
          llvm::Type::getInt1Ty(*context), false);
      case greater_tok:
        return builder->CreateIntCast(builder->CreateFCmpOGT(lhs_ir, rhs_ir, "fgthan"),
          llvm::Type::getInt1Ty(*context), false);
      case greater_eq_tok:
        return builder->CreateIntCast(builder->CreateFCmpOGE(lhs_ir, rhs_ir, "feqGthan"),
          llvm::Type::getInt1Ty(*context), false);
      case equal_tok:
        return builder->CreateIntCast(builder->CreateFCmpOEQ(lhs_ir, rhs_ir, "fequal"),
          llvm::Type::getInt1Ty(*context), false);
      default:
        return post_error_msg("Invalid operator");
    } 
    
  else {
    unsigned int lhs_bits {lhs_type->getIntegerBitWidth()};
    unsigned int rhs_bits {rhs_type->getIntegerBitWidth()};

    // do not type cast booleans
    if (lhs_bits != 1 && rhs_bits != 1) {
      if (lhs_bits > rhs_bits)
        rhs_ir = type_cast(rhs_ir, lhs_type, rhs_type);
      else if (rhs_bits > lhs_bits)
        lhs_ir = type_cast(lhs_ir, rhs_type, lhs_type);
    }

    if (!rhs || !lhs)
      return nullptr;

    switch (op) {
      case add_tok:
        return builder->CreateAdd(lhs_ir, rhs_ir, "add");
      case sub_tok:
        return builder->CreateSub(lhs_ir, rhs_ir, "sub");
      case div_tok:
        return builder->CreateSDiv(lhs_ir, rhs_ir, "div");
      case multi_tok:
        return builder->CreateMul(lhs_ir, rhs_ir, "multi");
      case and_tok:
        // didn't use is_bool(Value*) because bit size is already calculated above
        // thus no need to recalculate it in said function
        if (lhs_bits != 1 && rhs_bits != 1)
          return post_error_msg("Expected boolean value");
        return builder->CreateAnd(lhs_ir, rhs_ir, "and");
      case or_tok:
        if (lhs_bits != 1 && rhs_bits != 1)
          return post_error_msg("Expected boolean value");
        return builder->CreateOr(lhs_ir, rhs_ir, "or");
      case less_tok:
        return builder->CreateIntCast(builder->CreateICmpSLT(lhs_ir, rhs_ir, "lthan"),
          llvm::Type::getInt1Ty(*context), false);
      case less_eq_tok:
        return builder->CreateIntCast(builder->CreateICmpSLE(lhs_ir, rhs_ir, "eqLthan"),
          llvm::Type::getInt1Ty(*context), false);
      case greater_tok:
        return builder->CreateIntCast(builder->CreateICmpSGT(lhs_ir, rhs_ir, "gthan"),
          llvm::Type::getInt1Ty(*context), false);
      case greater_eq_tok:
        return builder->CreateIntCast(builder->CreateICmpSGE(lhs_ir, rhs_ir, "eqGthan"),
          llvm::Type::getInt1Ty(*context), false);
      case equal_tok:
        return builder->CreateIntCast(builder->CreateICmpEQ(lhs_ir, rhs_ir, "equal"),
          llvm::Type::getInt1Ty(*context), false);
      default:
        return post_error_msg("Invalid operator");
    }
  } 
}