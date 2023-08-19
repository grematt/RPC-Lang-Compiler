#include "../dot_h/expression_parser.h"
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
#include "../dot_h/ConstantArray.h"
#include "../dot_h/NotExpr.h"
#include "../dot_h/ir_utils.h"

std::unique_ptr<Expression> parse_not_expr() {
  get_next_tok();
  if (auto expr {parse_expression()})
    return std::make_unique<NotExpr>(std::move(expr));
  return nullptr;
}

std::unique_ptr<Expression> parse_constant_array() {
  std::vector<std::unique_ptr<Expression>> vals;
  int elem_type {};
  bool is_string {false};

  // brace initialization
  if (cur_token == '{') {
    get_next_tok();
    while (cur_token != '}') {
      auto index {parse_expression()};
      if (!index)
        return nullptr;
      
      // would be faster to put one iteration of the
      // loop before the while loop itself
      // to get type and skip this conditional,
      // but I will take the minor overhead in exchange
      // for cleaner code
      if (elem_type == 0)
        elem_type = index->type;

      if (index->type != elem_type)
        return post_error_msg("Braced arrays can only be initalized with same type literals");
      vals.push_back(std::move(index));
    }

  // double quotes string initialization
  } else if (cur_token == string_tok) {
    is_string = true;
    elem_type = ch8_tok;
    for (auto ch : token_string)
      vals.push_back(std::make_unique<Literal>(ch, ch8_tok));
  } else { 
    return post_error_msg("\'{\' or \'\"\' expected");
  }   

  // append null terminator for string functions
  if (is_string)
    vals.push_back(std::make_unique<Literal>('\0', ch8_tok));

  get_next_tok();
  return std::make_unique<ConstantArray>(std::move(vals), none, elem_type);
}

std::unique_ptr<Expression> parse_literal() {
  std::unique_ptr<Literal> ret;
  switch (cur_token) {
    case i32_tok:
      ret = std::make_unique<Literal>(token_int, cur_token);
      break;
    case f64_tok:
      ret = std::make_unique<Literal>(token_double, cur_token);
      break;
    case ch8_tok:
      ret = std::make_unique<Literal>(token_char, cur_token);
      break;
    default:
      return post_error_msg("Invalid literal type");
  }
  get_next_tok();
  return ret;
}

std::unique_ptr<Expression> parse_paren_expr() {
  if (cur_token != '(')
    post_error_msg("Expected '('");
  get_next_tok();

  auto expr = parse_expression();
  if (!expr)
    return nullptr;

  if (cur_token != ')')
    return post_error_msg("')' expected");
  get_next_tok();
  return expr;
}

std::unique_ptr<Expression> parse_return_expr() {
  get_next_tok();

  if (cur_token == void_ret_tok) {
    get_next_tok();
    return std::make_unique<ReturnExpr>(nullptr, void_ret_tok);
  }

  if (auto expr {parse_expression()})
    return std::make_unique<ReturnExpr>(std::move(expr), return_tok);
  return nullptr;
}

std::unique_ptr<Expression> parse_id_expr() {
  std::string id {token_string};
  get_next_tok();  // eat ID

  if (cur_token =='(') {  // function call
    get_next_tok();

    // parse function call args
    std::vector<std::unique_ptr<Expression>> args;
    while (cur_token != ')') {
      // variable
      if (cur_token == id_tok) {
        auto id {std::move(parse_id_expr())};
        if (!id)
          return nullptr;
        args.push_back(std::move(id));

      // constant array
      } else if (cur_token == '{' || cur_token == string_tok) {
        auto arr {parse_constant_array()};
        if (!arr)
          return nullptr;
        args.push_back(std::move(arr));

      // literal
      } else {
        while (cur_token != ')') {
          auto lit {std::move(parse_literal())};
          if (!lit)
            return nullptr;
          args.push_back(std::move(lit));
        }
      }
    }

    if (cur_token != ')')
      return post_error_msg("')'expected");
    get_next_tok();
    return std::make_unique<FuncCall>(id, none, std::move(args));
  }

  // prase either array sizes, or array indexes,
  // the process is the same, but it may be used for either
  std::vector<std::unique_ptr<Expression>> indices {};
  while (cur_token == '[') {
    get_next_tok();

    if (auto expr {parse_expression()})
      indices.push_back(std::move(expr));

    if (cur_token != ']')
      return post_error_msg("\']\' expected");
    get_next_tok();
  }

  // assignment expression
  if (cur_token == assignment_tok) {
    get_next_tok();
    auto rhs {parse_expression()};
    
    if (!rhs)
      post_error_msg("Expected expression after '<-'");
    return std::make_unique<AssignmentExpr>(id, std::move(rhs), std::move(indices));
  }

  // lone variable
  return std::make_unique<VarId>(id, none, std::move(indices));
}

std::unique_ptr<Expression> parse_primary_expr() {
  switch (cur_token) {
    case '(': 
      if (auto paren {parse_paren_expr()})
        return paren;
      return nullptr; 
    case id_tok:
      if (auto id {parse_id_expr()})
        return id;
      return nullptr;
    case i32_tok:
    case f64_tok:
    case ch8_tok:
      if (auto literal {parse_literal()})
        return literal;
      return nullptr;
    case '{':
    case string_tok:
      if (auto arr {parse_constant_array()})
        return arr;
      return nullptr;
    case not_tok:
      if (auto not_expr {parse_not_expr()})
        return not_expr;
      return nullptr;
    default:
      return post_error_msg("Expression expected");
  }
}

std::unique_ptr<Expression> parse_bin_expr_rhs(std::unique_ptr<Expression> 
                                    lhs, int prev_op_prec, bool is_arith_expr) {
  while (arith_or_bool_condition_eval(is_arith_expr)) {
    int cur_op_prec {get_precedence()};

    // same operator or that of less precedence
    if (cur_op_prec < prev_op_prec)
      return lhs;

    int op {cur_token};
    get_next_tok();
    auto rhs {parse_primary_expr()};

    if (!rhs)
      return nullptr;

    // +1 so expression is operated lhs -> rhs
    rhs = parse_bin_expr_rhs(std::move(rhs), cur_op_prec + 1, is_arith_expr);
    lhs = std::make_unique<BinaryExpr>(op, std::move(lhs), 
      std::move(rhs));
  }

  if (lhs->type == bin_expr_tok)
    lhs->type = none;
  return lhs;
}

std::unique_ptr<Expression> parse_bin_expr() {
  if (auto lhs {parse_primary_expr()}) {
    if (auto arith {parse_bin_expr_rhs(std::move(lhs), 0, true)}) {
      if (arith->type == bin_expr_tok) 
        return arith;
      // move back lhs for next if statement to use
      lhs = std::move(arith);
    }
    if (auto bool_expr {parse_bin_expr_rhs(std::move(lhs), 0, false)}) {
      if (bool_expr->type == bin_expr_tok)
        return bool_expr;
      lhs = std::move(bool_expr);
    }
    // expression is not a bin expr, so just return the primary expr parsed
    return lhs;
  }
  return nullptr;
}

// all expressions are checked for being a binary expression,
// as when the parse sees something like x, it cannot know if 
// the expression will be just x; or if it will be x + 1;
std::unique_ptr<Expression> parse_expression() {
  switch (cur_token) {
    default:
      if (auto binary {parse_bin_expr()})
        return binary;
      return nullptr;
  }
}