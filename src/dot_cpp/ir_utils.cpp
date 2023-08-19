#include "../dot_h/ir_utils.h"
#include "../dot_h/parser_utils.h"
#include "../dot_h/tokens.h"

bool is_return_expr = false;

std::nullptr_t post_error_msg(const char *error_msg) {
  fprintf(stderr, "Error: %s\n", error_msg);
  return nullptr;
}

llvm::Value* type_cast(llvm::Value* expr, llvm::Type* to_ty, llvm::Type* from_ty) {
  int from_id {from_ty->getTypeID()};
  int to_id {to_ty->getTypeID()};

  // make sure the 2 arays are compatable with each other.
  // if there are not they cannot be type casted as they are arrays,
  // so an error is returned
  if (from_id == llvm::Type::ArrayTyID &&
      to_id   == llvm::Type::ArrayTyID) {
    auto to_elem_type {to_ty->getArrayElementType()};
    auto from_elem_type {from_ty->getArrayElementType()};

    if (to_elem_type->isArrayTy() && from_elem_type->isArrayTy())
      return post_error_msg("Cannot assign arrays greater than 1 dimension to each other");
    if (to_elem_type != from_elem_type)
      return post_error_msg("Cannot assign arrays of different types to each other");

    auto to_len {to_ty->getArrayNumElements()};
    auto from_len {from_ty->getArrayNumElements()};

    if (to_len < from_len)
      return post_error_msg("Cannot assign larger array to smaller array");
    
    return expr;
  }

  // convert double to int
  if (from_id == llvm::Type::DoubleTyID && 
      to_id   == llvm::Type::IntegerTyID)
    return builder->CreateFPToSI(expr, to_ty, "conv");

  // up/down cast int to int, or convert int to double
  if (from_id == llvm::Type::IntegerTyID) {
    if (to_id == from_id)
      return builder->CreateIntCast(expr, to_ty, true, "upcast");
    else if (to_id == llvm::Type::DoubleTyID)
      return builder->CreateSIToFP(expr, to_ty, "conv");
  }

  return post_error_msg("Invalid type conversion");
}

bool create_ret(llvm::Value* statement_ir, llvm::Type* ret_type) {
  is_return_expr = false;
  
  auto statement_type {statement_ir->getType()};

  // a return expr returns int16 if void is returned.
  // this is arbitrary
  if (statement_type == builder->getInt16Ty()) {
    builder->CreateRet(llvm::UndefValue::get(builder->getVoidTy()));
    return true;
  }

  if (ret_type != statement_type)
    if (!(statement_ir = type_cast(statement_ir, ret_type, statement_type)))
      return false;

  statement_ir = create_array_upcast(ret_type, statement_type, statement_ir);
  if (!statement_ir)
    return false;

  builder->CreateRet(statement_ir);
  return true;
}

llvm::AllocaInst* allocate(std::string var_name, llvm::Type* type) {
  llvm::Value* array_size {nullptr};
  if (type->isArrayTy())
    array_size = llvm::ConstantInt::get(*context, llvm::APInt{32, 2});
  // the array_size param of CreateAlloca doesn't make sense to me,
  // if I enter the size of the array i get an error, if I enter 1,
  // meaning 1 array, I get an error, but as 2 works I'm just sticking with that.
  // I believe this means I allocate 2 of every array though which is not ideal.
  // If anyone has ideas, I'd appreciate it
  return builder->CreateAlloca(type, array_size, var_name);
}

void create_block_vars_entry(std::string& parent_name, std::string& child_name) {
  auto& parent_variables {block_vars[parent_name]};
  auto& new_vars {block_vars[child_name]};
  // copy all var from parent scope into child scope
  for (auto& var : parent_variables)
    new_vars[var.first] = var.second;
}

llvm::Type* tok_type_to_llvm_type_obj(int type, std::vector<int> array_indices) {
  switch (type) {
    case i32_tok:
    case i32_decl_tok:
      return builder->getInt32Ty();
    case f64_tok:
    case f64_decl_tok:
      return builder->getDoubleTy();
    case ch8_tok:
    case ch8_decl_tok:
      return builder->getInt8Ty();
    case ch8_arr_tok:
    case i32_arr_tok:
    case f64_arr_tok:
      return create_arr_type(array_indices, 
        tok_type_to_llvm_type_obj(arr_tok_to_from_prim_tok(type)));
    default:
      return post_error_msg("Invalid type");
  }
}

llvm::Type* create_arr_type(std::vector<int> array_sizes, llvm::Type* elem_type) {
  // create an array type from the right to left of declaration. For example:
  // If you write i32[4][3][2], the function will make [2 x i32] type, then
  // [3 x [i32 x 2]] type, then [4 x [3 x [i32 x 2]]] type.   
  llvm::Type* prev_type {llvm::ArrayType::get(elem_type, array_sizes[array_sizes.size() - 1])};
  for (auto it {array_sizes.rbegin() + 1}; it != array_sizes.rend(); ++it) {
    llvm::Type* cur_type {llvm::ArrayType::get(prev_type, *it)};
    prev_type = cur_type;
  }
  return prev_type;
}

std::unique_ptr<std::pair<llvm::Type*, llvm::Value*>> create_gep(
                    std::vector<std::unique_ptr<Expression>> arr_indices, 
                    llvm::Type* type, llvm::Value* var, std::string name) {
  // generate ir for each value used for indexing the arr
  std::vector<llvm::Value*> indecies_ir;
  for (auto& i : arr_indices) {
    if (auto array_index_ir {i->generate_ir()})
      indecies_ir.push_back(array_index_ir);
    else
      return post_error_msg("Invalid array index");
  }

  // insert a 0 at the beginning, just a quirk of the GEP instruction
  indecies_ir.insert(indecies_ir.begin(), llvm::ConstantInt::get(*context, 
    llvm::APInt{32, 0}));
    
  auto gep {builder->CreateGEP(type, var, indecies_ir, name + "-deref")};

  // load type is used by some other functions just to see what
  // type is being returned by the GEP instruction
  auto load_type {type};

  size_t num_nested_indices {arr_indices.size()};
  for (int i {}; i < num_nested_indices; ++i)
    load_type = load_type->getArrayElementType();
  
  return std::make_unique<std::pair<llvm::Type*, llvm::Value*>>(load_type, gep);
}

bool is_bool(llvm::Value* operand) {
  auto type {operand->getType()};
  if (type->getTypeID() == llvm::Type::IntegerTyID && type->getIntegerBitWidth() == 1)
    return true;
  post_error_msg("Only boolean values can use the \"and\", \"or\", \"not\" operators");
  return false;
}

std::pair<bool, bool> generate_statement_body(
  std::vector<std::unique_ptr<Statement>> body, llvm::Type*& ret_type) {
  bool is_return {false};
  for (auto& statement : body) {
    auto statement_ir {statement->generate_ir()};
    if (!statement_ir)
      return {false, false};
    if (is_return_expr) {
      if (!create_ret(statement_ir, ret_type))
        return {false, false};
      is_return = true;
      break;
    }
  }
  return {true, is_return};
}

llvm::BasicBlock* generate_block_ir(std::string& parent_name,
                                    std::string& block_name,
                                    llvm::BasicBlock* block,
                                    llvm::BasicBlock*next_block,
                                    llvm::Function* parent_func,
                                    std::vector<std::unique_ptr<Statement>> body) {
  parent_func->insert(parent_func->end(), block);
  builder->SetInsertPoint(block);

  if (body.size())
    create_block_vars_entry(parent_name, block_name);

  auto ret_type {parent_func->getReturnType()};
  
  auto body_ir_gen_result {generate_statement_body(std::move(body), ret_type)};

  // generation failed
  if (!body_ir_gen_result.first)
    return nullptr;

  // if there is not a return expr
  if (!body_ir_gen_result.second)
    builder->CreateBr(next_block);
  return builder->GetInsertBlock();
}

llvm::Value* create_array_upcast(llvm::Type* recorded_type, 
                                 llvm::Type* arg_type, 
                                 llvm::Value* arg_code) {
  // if arg_code isn't an array, or if it is > 1 dimension, do nothing
  if (!recorded_type->isArrayTy()           || 
      !arg_type->isArrayTy()                ||
      arg_type->getNumContainedTypes() != 1 ||
      arg_type->getNumContainedTypes() != 1)
    return arg_code;

  auto arg_len {arg_type->getArrayNumElements()};
  auto recorded_len {recorded_type->getArrayNumElements()};

  // don't cast arrays of equal size, and a larger array
  // cannot be casted to a smaller array
  if (arg_len < recorded_len) {
    // create larger array
    auto new_arr {allocate("", recorded_type)};
    auto gep {create_gep({}, recorded_type, new_arr)->second};
    if (!gep)
      return nullptr;
    
    // store smaller array in larger, temp array
    builder->CreateStore(arg_code, gep);
    return builder->CreateLoad(recorded_type, gep, "upcasted_arr");
  }
  return arg_code;
}