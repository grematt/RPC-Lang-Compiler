#ifndef IR_UTILS_H
#define IR_UTILS_H

#include <string>
#include "llvm-includes.h"
#include "Expression.h"

extern bool is_return_expr;

// prints an error message to std error
std::nullptr_t post_error_msg(const char* error_msg);

// casts a Value* to a different type
llvm::Value* type_cast(llvm::Value* target, llvm::Type* to_ty, llvm::Type* from_ty);

// creates a return instruction of the given statement and type
bool create_ret(llvm::Value* statement_ir, llvm::Type* ret_type);

// allocates memory to a variable
llvm::AllocaInst* allocate(std::string var_name, llvm::Type* type);

// creates a new variable lookup table for a given block
void create_block_vars_entry(std::string& parent_name, std::string& child_name);

// convert a token of a type (i32_tok, ect) to an llvm::Type object
llvm::Type* tok_type_to_llvm_type_obj(int type, std::vector<int> array_indices = {});

// create an llvm::ArrayType, which may contain nested arrays, of the given
// element type
llvm::Type* create_arr_type(std::vector<int> array_sizes, llvm::Type* elem_type);

// create a GEP instruction, used for accessing arrays
std::unique_ptr<std::pair<llvm::Type*, llvm::Value*>> create_gep(
  std::vector<std::unique_ptr<Expression>> arr_indices, 
  llvm::Type* type, llvm::Value* var, std::string name = "");

// returns if a value pointer is a 1 bit integer type
bool is_bool(llvm::Value* operand);

// generate IR for the body of loops, functions, and if/otherwise statements
// returns {false, _} if error, returns {true, is_ret} on a success,
// is_ret signals if a return expr returns
std::pair<bool, bool> generate_statement_body(
  std::vector<std::unique_ptr<Statement>> body, llvm::Type*& ret_type);

// setup a basic block for ir generation, then call generate_statement_body
llvm::BasicBlock* generate_block_ir(std::string& parent_name,
                                    std::string& block_name,
                                    llvm::BasicBlock* block,
                                    llvm::BasicBlock*next_block,
                                    llvm::Function* parent_func,
                                    std::vector<std::unique_ptr<Statement>> body);

// cast a smaller 1d array to a larger 1d array
llvm::Value* create_array_upcast(llvm::Type* recorded_type, 
                                 llvm::Type* arg_type, 
                                 llvm::Value* arg_code);

#endif