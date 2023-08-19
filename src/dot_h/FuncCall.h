#ifndef FUNC_CALL_H
#define FUNC_CALL_H

#include "Expression.h"
#include <string>

class FuncCall : public Expression {
  std::string func_name;
  std::vector<std::unique_ptr<Expression>> args;

public:
  llvm::Value* generate_ir() override;
  FuncCall(std::string& func_name, int type,
    std::vector<std::unique_ptr<Expression>> args) :
    func_name{func_name}, Expression{type}, args{std::move(args)} {}  
};

#endif