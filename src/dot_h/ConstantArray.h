#ifndef CONSTANT_ARRAY_H
#define CONSTANT_ARRAY_H

#include "Expression.h"

class ConstantArray : public Expression {
    std::vector<std::unique_ptr<Expression>> vals;
    int elem_type;

public:
  llvm::Value* generate_ir() override;
  ConstantArray(std::vector<std::unique_ptr<Expression>> vals, int type, int elem_type) : 
    vals{std::move(vals)}, Expression{type}, elem_type{elem_type} {}
};

#endif