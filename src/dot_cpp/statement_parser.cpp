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
#include "../dot_h/expression_parser.h"
#include "../dot_h/tokens.h"
#include "../dot_h/ir_utils.h"

std::unique_ptr<std::vector<std::unique_ptr<Statement>>> parse_statement_body() {
  if (cur_token != '{')
    post_error_msg("Expected '{'");
  get_next_tok();

  std::vector<std::unique_ptr<Statement>> body;
  while (cur_token != '}') {
    if (auto statement {parse_statement()})
      body.push_back(std::move(statement));
    else 
      return nullptr;
  }

  get_next_tok();
  return std::make_unique<std::vector<std::unique_ptr<Statement>>>(std::move(body));
}

std::unique_ptr<Statement> parse_if_statement() {
  get_next_tok();
  
  auto condition {parse_paren_expr()};
  if (!condition)
    return nullptr;

  auto body {parse_statement_body()};
  if (!body)
    return nullptr;

  std::vector<std::unique_ptr<Statement>> otherwise_body;

  if (cur_token == otherwise_tok) {
    get_next_tok();
    auto temp {parse_statement_body()};
    if (!temp)
      return nullptr;
    otherwise_body = std::move(*temp);
  }

  return std::make_unique<IfStatement>(std::move(condition), 
    std::move(*body), std::move(otherwise_body));
}

std::unique_ptr<Statement> parse_end_with_semi_colon() {
  std::unique_ptr<Statement> statement;
  switch (cur_token) {
    case return_tok:
      if (statement = parse_return_expr())
        break;
      return nullptr;
    case var_dec_tok:
      if (statement = parse_variable_declaration())
        break;
      return nullptr;
    default:
      if (statement = parse_expression())
        break;
      return post_error_msg("Statement expected");
  }
  if (cur_token != ';')
    return post_error_msg("Expected ';'");
  get_next_tok();
  return statement;
}

std::unique_ptr<Statement> parse_for_loop() {
  get_next_tok();  // skip "for"

  if (cur_token != '(')
    return post_error_msg("'(' Expected");
  get_next_tok();

  auto init_val {parse_end_with_semi_colon()};
  if (!init_val)
    return nullptr;

  auto condition {parse_expression()};
  if (!condition)
    return nullptr;
  
  if (cur_token != ';')
    return post_error_msg("';' expected");
  get_next_tok(); 

  auto post_loop_expr {parse_expression()};
  if (!post_loop_expr)
    return nullptr;

  if (cur_token != ')')
    return post_error_msg("')' Expected");
  get_next_tok();
  
  auto body {parse_statement_body()};
  if (!body)
    return nullptr;
  
  body->push_back(std::move(post_loop_expr));

  auto while_loop {std::make_unique<WhileLoopStatement>(std::move(condition),
                                                           std::move(*body))};

  if (while_loop)
    return std::make_unique<ForLoopStatement>(std::move(init_val), std::move(while_loop));
  return nullptr;
}

std::unique_ptr<Statement> parse_statement() {
  switch (cur_token) {
    case if_tok:
      if (auto if_stmnt {parse_if_statement()})
        return if_stmnt;
      return post_error_msg("Problem in if statement");
    case while_tok:
      if (auto while_stmnt {parse_while_loop()})
        return while_stmnt;
      return post_error_msg("Problem in while statement");
    case for_tok:
      if (auto for_stmnt {parse_for_loop()})
        return for_stmnt;
      return post_error_msg("Problem in for statement");
    default:
      if (auto expr {parse_end_with_semi_colon()})
        return expr;
      return post_error_msg("Statement expected");
  }
}

std::unique_ptr<FuncDefinition> parse_func_definition() {
  get_next_tok();  // skip "def"
  if (cur_token != func_tok) 
    return post_error_msg("\"func\" expected");
  
  auto declaration {parse_func_declaration()};
  if (cur_token != '{') 
    return post_error_msg("'{' expected");
  
  get_next_tok();

  // parse until end of function
  std::vector<std::unique_ptr<Statement>> body;
  while (cur_token != '}') {
    if (auto statement {parse_statement()})
      body.push_back(std::move(statement));
    else 
      return post_error_msg("Invalid statement returned");
  }
  
  return std::make_unique<FuncDefinition>(std::move(declaration), std::move(body));
}

std::unique_ptr<Statement> parse_primative_var_dec(bool is_global, int type_tok) {
  if (cur_token != id_tok)
    return post_error_msg("ID expected");
  std::string name {token_string};
  get_next_tok();

  // vars must be initialized
  if (cur_token != assignment_tok)
    return post_error_msg("'<-' expected");
  get_next_tok();

  auto init_val {parse_expression()};
  if (!init_val)
    return nullptr;

  return std::make_unique<PrimativeVarDec>(type_tok, name, std::move(init_val), is_global);
}

std::unique_ptr<std::vector<int>> parse_array_sizes_int() {
  auto nested_array_sizes {std::make_unique<std::vector<int>>()};

  // if array is more than 1d, get sizes
  while (cur_token == '[') {
    get_next_tok();

    if (cur_token != i32_tok)
      return post_error_msg("Array size must be an integer value");
    nested_array_sizes->push_back(token_int);
    get_next_tok();

    if (cur_token != ']')
      return post_error_msg("\']\' expected");
    get_next_tok();
  }

  return nested_array_sizes;
}

std::unique_ptr<Statement> parse_array_var_dec(bool is_global, int type_tok) {
  auto nested_array_sizes {std::move(*parse_array_sizes_int())};

  if (cur_token != id_tok)
    return post_error_msg("ID expected");
  std::string name {token_string};
  get_next_tok();

  return std::make_unique<ArrayVarDec>(type_tok, name, nested_array_sizes, is_global);
}

std::unique_ptr<Statement> parse_variable_declaration(bool is_global /* = false */) {
  get_next_tok();  // skip "var"

  std::unique_ptr<Statement> ret;
  int type_tok {cur_token};
  get_next_tok();

  if (is_prim_decl_tok(type_tok))
    ret = parse_primative_var_dec(is_global, type_tok);
  else if (is_arr_decl_tok(type_tok))
    ret = parse_array_var_dec(is_global, type_tok);
  else
    return post_error_msg("Variable declared of unknown type");

  // non globals have ; checking done in parse_statement,
  // globals have it done here as expressions do not require
  // them by in the grammar
  if (is_global && cur_token != ';')
    return post_error_msg("\';\' expected");
  return ret;
}

std::unique_ptr<Statement> parse_while_loop() {
  get_next_tok();  // skip "while"
  auto condition {parse_paren_expr()};
  if (!condition)
    return nullptr;
  
  auto body {parse_statement_body()};
  if (!body)
    return nullptr;

  return std::make_unique<WhileLoopStatement>(std::move(condition),
                                              std::move(*body));
}

std::unique_ptr<FuncDeclaration> parse_func_declaration() {
  get_next_tok();  // skip over "func"

  if (cur_token != id_tok)
    return post_error_msg("Function name expected");
  std::string name {token_string}; 
  get_next_tok();

  if (cur_token != '(')
    return post_error_msg("'(' Expected");
  get_next_tok();  // get parameters

  // every parameter needs a type and name
  std::vector<std::unique_ptr<VarId>> params;
  while (is_prim_decl_tok(cur_token) || is_arr_decl_tok(cur_token)) {
    int type {cur_token};
    get_next_tok();

    // parse array sizes, and empty array is retuned if
    // a parameter is not an array
    auto array_sizes {std::move(*parse_array_sizes_int())};
    
    if (cur_token != id_tok)
      return post_error_msg("Parameter name expected");
    std::string varName {token_string}; 
    get_next_tok();
    
    params.push_back(std::make_unique<VarId>(varName, type, std::move(array_sizes)));
  }

  if (cur_token != ')')
    return post_error_msg("')' Expected");
  get_next_tok();

  // get return type
  if (!is_arr_decl_tok(cur_token)  && 
      !is_prim_decl_tok(cur_token) && 
      cur_token != void_ret_tok)
    return post_error_msg("Invalid return type");

  int ret_type {cur_token};
  get_next_tok();

  // if ret type is array, get the sizes of each array
  std::vector<int> arr_size {};
  if (is_arr_decl_tok(ret_type))
    arr_size = std::move(*parse_array_sizes_int());

  return std::make_unique<FuncDeclaration>(name, ret_type, std::move(params), arr_size);
}