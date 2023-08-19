#include "../dot_h/ForLoopStatement.h"
#include "../dot_h/ir_utils.h"

// a for loop is just a while loop with a temporary block
// placed before hand to delcare the loop varaible.
llvm::Value* ForLoopStatement::generate_ir() {
  static int id;  // used for inter-IR naming

  // create temp block 

  auto parent_name {builder->GetInsertBlock()->getName().str()};
  std::string loop_init_name {"for_loop_init" + std::to_string(id++)};
  llvm::Function* parent_func {builder->GetInsertBlock()->getParent()};

  llvm::BasicBlock* init_block {llvm::BasicBlock::Create(*context, loop_init_name)};
  parent_func->insert(parent_func->end(), init_block);
  create_block_vars_entry(parent_name, loop_init_name);
  builder->CreateBr(init_block);
  builder->SetInsertPoint(init_block);

  if (!init_statement->generate_ir())
    return nullptr;

  auto loop_ir {while_loop->generate_ir()};
  if (!loop_ir)
    return nullptr;

  // delete the loop var at the end of the loop so its scope
  // does not extend into the next block

  auto& parent_var_table {block_vars[parent_name]};
  auto& init_var_table {block_vars[loop_init_name]};
  std::string cur_name {builder->GetInsertBlock()->getName().str()};
  
  for (auto& var : init_var_table)
    if (parent_var_table[var.first] == nullptr)
      block_vars[cur_name].erase(var.first);

  return loop_ir;
}