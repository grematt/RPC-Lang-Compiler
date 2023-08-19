#ifndef ASSIGNMENT_EXPR_H
#define ASSIGNMENT_EXPR_H

#include "Expression.h"
#include <string>
#include <vector>
#include "tokens.h"

class AssignmentExpr : public Expression {
  std::string lhs_name;
  std::unique_ptr<Expression> rhs;
  // if the lhs is an array, lhs_arr_indices contains the list of derefs on lhs
  std::vector<std::unique_ptr<Expression>> lhs_arr_indices;

public:
  AssignmentExpr(std::string lhs_name, std::unique_ptr<Expression> rhs, 
    std::vector<std::unique_ptr<Expression>> lhs_arr_indices = {}) :
    lhs_name{lhs_name}, rhs{std::move(rhs)}, Expression(none), 
    lhs_arr_indices{std::move(lhs_arr_indices)} {}
  llvm::Value* generate_ir() override;
};

#endif