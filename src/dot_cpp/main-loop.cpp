#include "../dot_h/statement_parser.h"
#include "../dot_h/parser_utils.h"
#include "../dot_h/FuncDefinition.h"
#include "../dot_h/ForLoopStatement.h"
#include "../dot_h/WhileLoopStatement.h"
#include "../dot_h/Literal.h"
#include "../dot_h/FuncCall.h"
#include "../dot_h/AssignmentExpr.h"
#include "../dot_h/BinaryExpr.h"
#include "../dot_h/PrimativeVarDec.h"
#include "../dot_h/ArrayVarDec.h"
#include "../dot_h/IfStatement.h"
#include "../dot_h/ReturnExpr.h"
#include "../dot_h/tokens.h"
#include "../dot_h/ir_utils.h"
#include <iostream>
#include <string>

std::unique_ptr<llvm::LLVMContext> context {};
std::unique_ptr<llvm::IRBuilder<>> builder {}; 
std::unique_ptr<llvm::Module> module {};
std::map<std::string, std::map<std::string, llvm::AllocaInst*>> block_vars {};

static void handle_func_declaration();
static void handle_func_definition();
static void handle_global_definition();
static void init_module();
void init_precedence();
int generate_output_file(std::string fname);

void main_loop(std::string fname) {
  while(1) {
    get_next_tok(fname); 
    switch (cur_token) {
      case eof_tok:
        return;
      case def_tok:
        handle_func_definition();
        break;
      case func_tok:
        handle_func_declaration();
        break;
      case var_dec_tok:
        handle_global_definition();
        break;
      default:
        break;
    }
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    post_error_msg("File name required for compilation");
    return 1;
  }

  bool ir_flag {false}; 
  if (argc == 3) {
    std::string ir {argv[2]};
    if (ir == "-ir")
      ir_flag = true;
    else {
      post_error_msg("Invalid compiler arguments");
      return 1;
    }
  }

  if (argc > 3) {
    post_error_msg("Invalid compiler arguments");
    return 1;
  }

  std::string fname {argv[1]};
  if (fname.find(".txt") == std::string::npos) {
    post_error_msg("Input file must be a text file");
    return 1;
  }

  init_precedence();
  init_module();

  main_loop(fname);

  if (ir_flag)
    module->print(llvm::errs(), nullptr);
  return generate_output_file(fname);
}

static void handle_func_declaration() {
  auto temp {parse_func_declaration()}; 
  if (!temp || cur_token != ';' || !temp->generate_ir()) {
    std::cout << "Error parsing function declaration" << std::endl;
    std::exit(1);
  } 
}

static void handle_func_definition() {
  auto temp {parse_func_definition()};
  if (!temp || !temp->generate_ir()) {
    std::cout << "Error parsing function definition" << std::endl;
    std::exit(1);
  }
}

static void handle_global_definition() {
  auto temp {parse_variable_declaration(true)};
  if (!temp || !temp->generate_ir()) {
    std::cout << "Error parsing global definition" << std::endl;
    std::exit(1);
  }
}

static void init_module() {
  // Open a new context and module.
  context = std::make_unique<llvm::LLVMContext>();
  module = std::make_unique<llvm::Module>("RPC", *context);

  // Create a new builder for the module.
  builder = std::make_unique<llvm::IRBuilder<>>(*context);
}


void init_precedence() {
  operator_precedence[or_tok]         = 200;
  operator_precedence[and_tok]        = 250;
  operator_precedence[greater_eq_tok] = 300;
  operator_precedence[less_eq_tok]    = 300;
  operator_precedence[less_tok]       = 300;
  operator_precedence[greater_tok]    = 300;
  operator_precedence[equal_tok]      = 300;
  operator_precedence[add_tok]        = 400;
  operator_precedence[sub_tok]        = 400;
  operator_precedence[div_tok]        = 500;
  operator_precedence[multi_tok]      = 500;
}

int generate_output_file(std::string fname) {
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  auto target_triple = llvm::sys::getDefaultTargetTriple();
  module->setTargetTriple(target_triple);

  std::string Error;
  auto compiler_target = llvm::TargetRegistry::lookupTarget(
                              target_triple, Error);

  llvm::TargetOptions opt;
  auto RM = std::optional<llvm::Reloc::Model>();
  auto target = compiler_target->createTargetMachine(
                                       target_triple, 
                              std::string("generic"), 
                                     std::string(""), 
                               llvm::TargetOptions{}, 
                                                 RM);

  module->setDataLayout(target->createDataLayout());

  fname = fname.substr(0, fname.find(".txt"));
  fname.append(".o");

  std::error_code ec;
  llvm::raw_fd_ostream dest(fname, ec, llvm::sys::fs::OF_None);

  if (ec) {
    llvm::errs() << "Could not generate output file: " << ec.message();
    return 1;
  }

  llvm::legacy::PassManager pass;
  auto file_type = llvm::CGFT_ObjectFile;

  if (target->addPassesToEmitFile(pass, dest, nullptr, file_type)) {
    llvm::errs() << "can't generate file of this type \n";
    return 1;
  }

  pass.run(*module);
  dest.flush();

  return 0;
}